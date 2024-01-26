import typing
from enum import Enum
from copy import deepcopy

SYMBOLS = '∀∃|&¬→='


class OpType(Enum):
    ALL = 0
    EXISTS = 1
    OR = 2
    AND = 3
    NOT = 4
    IMPLY = 5
    EQUAL = 6


def sym2type(sym):
    idx = SYMBOLS.index(sym)
    return OpType(idx)


class Quantifier:
    def __init__(self, op_type: OpType, var, op):
        assert op_type in [OpType.ALL, OpType.EXISTS]
        self.type = op_type
        self.var = var
        self.op = op

    def walk(self):
        for x in self.op.walk():
            yield x
        yield self.op

    def negate(self):
        self.type = OpType.EXISTS if self.type == OpType.ALL else OpType.ALL
        self.op = Operation(OpType.NOT, [self.op])
        return self

    def rename_var(self, old_name, new_name):
        self.op.rename_var(old_name, new_name)

    def __str__(self):
        if self.op is None:
            return SYMBOLS[self.type.value] + f'{self.var}'
        return SYMBOLS[self.type.value] + f'{self.var} ({self.op})'

    def __repr__(self):
        return str(self)


class Operation:
    def __init__(self, op_type: OpType, args: typing.List):
        assert op_type not in [OpType.ALL, OpType.EXISTS]
        self.type = op_type
        self.args = args

    def walk(self):
        for arg in self.args:
            for x in arg.walk():
                yield x
            yield arg

    def negate(self):
        assert self.type in [OpType.OR, OpType.AND, OpType.NOT]
        if self.type == OpType.NOT:
            return self.args[0]

        self.type = OpType.OR if self.type == OpType.AND else OpType.AND
        for i, arg in enumerate(self.args):
            self.args[i] = Operation(OpType.NOT, [self.args[i]])
        return self

    def rename_var(self, old_name, new_name):
        for arg in self.args:
            arg.rename_var(old_name, new_name)

    def __str__(self):
        def need_brackets(arg):
            return not isinstance(arg, Variable) \
                and not isinstance(arg, Predicate) \
                and not (isinstance(arg, Operation) and arg.type == OpType.NOT and self.type != OpType.NOT)

        def add_opt_brackets(arg):
            if need_brackets(arg):
                return f'({arg})'
            return str(arg)

        if self.type != OpType.NOT:
            return f' {SYMBOLS[self.type.value]} '.join(map(add_opt_brackets, self.args))
        else:
            a1 = add_opt_brackets(self.args[0])
            return f'¬{a1}'

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        if not isinstance(other, Operation):
            return False
        if self.type != other.type or len(self.args) != len(other.args):
            return False
        for arg1, arg2 in zip(self.args, other.args):
            if arg1 != arg2:
                return False
        return True

    def __deepcopy__(self, memo):
        cls = self.__class__
        result = cls.__new__(cls)
        memo[id(self)] = result
        for k, v in self.__dict__.items():
            setattr(result, k, deepcopy(v, memo))
        return result


class Variable:
    def __init__(self, name, negative=False):
        self.name = name
        self.negative = negative

    @property
    def is_const(self):
        return not self.is_var

    @property
    def is_var(self):
        return self.name[0].islower()

    @staticmethod
    def walk():
        return []

    def negate(self):
        self.negative = not self.negative
        return self

    def rename_var(self, old_name: str, new_name: str, to_const: bool = False):
        if self.name == old_name:
            self.name = new_name

    def __str__(self):
        return self.name if not self.negative else f'¬{self.name}'

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        if not isinstance(other, Variable):
            return False
        return self.name == other.name and self.negative == other.negative

    def __hash__(self):
        return hash(tuple([self.name, self.negative]))

    def __deepcopy__(self, memo):
        cls = self.__class__
        result = cls.__new__(cls)
        memo[id(self)] = result
        for k, v in self.__dict__.items():
            setattr(result, k, deepcopy(v, memo))
        return result


class Predicate:
    def __init__(self, name, args: typing.List[Variable], negative=False):
        self.name = name
        self.args = args
        self.negative = negative

    @staticmethod
    def walk():
        return []

    def negate(self):
        self.negative = not self.negative
        return self

    def rename_var(self, old_name: str, new_name: str, to_const: bool = False):
        for arg in self.args:
            arg.rename_var(old_name, new_name, to_const)

    def __str__(self):
        x = f'{self.name}({", ".join([str(x) for x in self.args])})'
        return x if not self.negative else f'¬{x}'

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        if not isinstance(other, Predicate):
            return False
        if self.name != other.name or len(self.args) != len(other.args):
            return False
        if self.negative != other.negative:
            return False
        for arg1, arg2 in zip(self.args, other.args):
            if arg1 != arg2:
                return False
        return True

    def __hash__(self):
        return hash(tuple([self.name, self.negative, *self.args]))

    def __deepcopy__(self, memo):
        cls = self.__class__
        result = cls.__new__(cls)
        memo[id(self)] = result
        for k, v in self.__dict__.items():
            setattr(result, k, deepcopy(v, memo))
        return result


class Rule:
    """
    Логическое выражение
    вида x1 & ... & xn -> y1
    """

    def __init__(self, imply_op):
        assert imply_op.type == OpType.IMPLY

        self.op = imply_op

        # доказываемый терм
        self.out_term = imply_op.args[1]

        # входные термы
        in_terms = imply_op.args[0]
        if isinstance(in_terms, Operation):
            assert in_terms.type == OpType.AND
            self.in_terms = in_terms.args
        elif isinstance(in_terms, Predicate):
            self.in_terms = [in_terms]

        self.approved = False

    @property
    def atoms(self):
        return self.op.args

    def rename_var(self, old_name: str, new_name: str, to_const: bool):
        for a in self.atoms:
            for arg in a.args:
                arg.rename_var(old_name, new_name, to_const)

    def __str__(self):
        return str(self.op)

    def __repr__(self):
        return str(self)

    def __eq__(self, other):
        return self.op == other.op

    def __hash__(self):
        return hash(self.op)

    def __deepcopy__(self, memo):
        cls = self.__class__
        result = cls.__new__(cls)
        memo[id(self)] = result
        for k, v in self.__dict__.items():
            setattr(result, k, deepcopy(v, memo))
        return result
