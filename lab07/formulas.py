import copy
from collections import defaultdict
from parser import *
from models import *


def normalize_formula(op: Operation):
    # 1 - избавиться от → и =
    # Идём по всем элементам
    for x in op.walk():
        if isinstance(x, Operation):
            # x → y = ¬x | y
            if x.type == OpType.IMPLY:
                x.type = OpType.OR
                x.args[0] = Operation(OpType.NOT, [x.args[0]])
            # x = y = (x & y) | (¬x & ¬y)
            elif x.type == OpType.EQUAL:
                x.type = OpType.OR
                a, b = x.args
                x.args[0] = Operation(OpType.AND, [a, b])
                x.args[1] = Operation(OpType.AND, [
                    Operation(OpType.NOT, [copy.deepcopy(a)]),
                    Operation(OpType.NOT, [copy.deepcopy(b)])
                ])
    print(f'1. Без → и =:\t', op)

    # 2 - пронести отрицание до атомов
    def remove_not(op):
        def is_not(op):
            return isinstance(op, Operation) and op.type == OpType.NOT

        # Если переменная или предикат, то конец
        if isinstance(op, Variable) or isinstance(op, Predicate):
            return op

        # если квантор
        elif isinstance(op, Quantifier):
            op.op = remove_not(op.op)

        # если операция отрицания
        elif is_not(op):
            op.args[0] = op.args[0].negate()
            return remove_not(op.args[0])
        # обычная операция
        else:
            for i in range(len(op.args)):
                op.args[i] = remove_not(op.args[i])
        return op

    op = remove_not(op)
    print(f'2. де Морган:\t', op)

    # 3. переименование связанных переменных
    prefix = []  # для кванторов
    var_rename_counter = defaultdict(int)

    def rename_vars(op):
        if isinstance(op, Variable) or isinstance(op, Predicate):
            return op
        elif isinstance(op, Operation):
            for i in range(len(op.args)):
                op.args[i] = rename_vars(op.args[i])
        # Нашли квантор
        else:
            var_name = op.var.name
            var_rename_counter[var_name] += 1

            new_name = f'{var_name}{var_rename_counter[var_name]}'
            op.op.rename_var(var_name, new_name)
            prefix.append(Quantifier(op.type, new_name, None))
            return rename_vars(op.op)
        return op

    op = rename_vars(op)
    pretty_prefix = ' '.join([str(x) for x in prefix])
    print(f'3. Без кванторов:', pretty_prefix, op)

    # 4. преобразование к КНФ (дистрибутивность сверху вниз, пока | не будет только с примитивами)
    def is_disjunct(op):
        if isinstance(op, Variable) or isinstance(op, Predicate):
            return True
        if op.type == OpType.AND:
            return False
        return is_disjunct(op.args[0]) and is_disjunct(op.args[1])

    def make_disjuncts(op):
        if is_disjunct(op):
            return
        if op.type == OpType.AND:  # все ок
            for arg in op.args:
                make_disjuncts(arg)
            return
        a, b = op.args
        if is_disjunct(a) and is_disjunct(b):  # тоже ок
            return
        # иначе дистрибутивность
        if is_disjunct(a):
            a, b = b, a  # конъюнкцию помещаем в a
        op.type = OpType.AND
        op.args[0] = Operation(OpType.OR, [a.args[0], b])
        op.args[1] = Operation(OpType.OR, [a.args[1], b])
        for arg in op.args:
            make_disjuncts(arg)
        return

    make_disjuncts(op)
    print(f'4. КНФ:\t', op)

    # 5 - извлечение дизъюнктов
    disjuncts = []

    def simplify_disjunct(op):
        vars = dict()
        if isinstance(op, Variable) or isinstance(op, Predicate):
            raw_vars = [op]
        else:
            raw_vars = list(op.walk())
        for x in raw_vars:
            if isinstance(x, Variable) or isinstance(x, Predicate):
                neg = x.negative
                if isinstance(x, Variable):
                    name = x.name
                else:
                    name = x.name + '_' + '_'.join(
                        str(a) for a in x.args)  # чтобы отличать предикаты с разными переменными
                if vars.get(name) is None:
                    vars[name] = [x, set([neg])]
                else:
                    vars[name][1].add(neg)
        vars = [var for k, (var, s) in vars.items() if len(s) == 1]
        return vars

    def extract_disjuncts(op):
        if is_disjunct(op):
            op = simplify_disjunct(op)
            if len(op) > 0:
                disjuncts.append(op)
            return
        a, b = op.args
        extract_disjuncts(a)
        extract_disjuncts(b)

    extract_disjuncts(op)

    print('5. Извлечённые дизъюнкты:')
    for i, dj in enumerate(disjuncts):
        print(f'\t{i + 1}. ' + ' | '.join([str(d) for d in dj]))
    print('\n')

    return prefix, disjuncts


if __name__ == '__main__':
    # res = formula.parseString('∀x ((p1(y) → p2(x, y)) → (p2(x, y))) | p2(x, y)')[0]
    res = formula.parseString('¬(x & y & z)')[0]
    print(res)

    res = normalize_formula(res)
    # print(res)
