import copy


class Rule:
    def __init__(self, input_atoms, output_vertex):
        self.input_atoms = input_atoms
        self.output_vertex = output_vertex


class Atom:
    def __init__(self, name, terminals):
        self.name = name
        self.terminals = terminals

    def __str__(self):
        strterms = ""
        for term in self.terminals:
            strterms += str(term) + ", "
        return self.name + '(' + strterms.strip(", ") + ')'

    def __repr__(self):
        return self.__str__()


class Constant:  # структура константа
    def __init__(self, value):
        self.value = value  # значение
        self.variable = False  # флаг НЕ переменная

    def __str__(self):
        return self.value

    def __repr__(self):
        return self.__str__()


class Variable:  # структура переменная
    def __init__(self, name):
        self.name = name  # имя
        self.variable = True  # флаг переменная

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.__str__()


# структура подстановок (таблица подстановок: какие значения присвоены переменным и какие переменные связаны между собой.)
class Table:
    def __init__(self):
        self.variables = dict()  # храним переменные
        self.links = dict()  # связанные значения

    # Вывод на экран и вывод ошибок
    def reset(self, other):
        self.variables = other.variables
        self.links = other.links

    def val(self, var):
        return self.variables[var.name]

    def var_links(self, var):
        return self.links[self.variables[var.name]]

    def __str__(self):
        res = ""
        for const in self.links.keys():
            res += str(self.links[const]) + ": " + str(const) + "\n"
        return res


def unification(table, p1, p2):  # получаеем атомы которые нужно унифицировать
    # Проверяем, равны имена предикатов p1,p2 или нет
    if p1.name != p2.name:
        # Если имена не равны, то унификация невозможна
        print('Имена не совпадают')
        return False

    # Проверяем, равны ли длины массивов переменных предикатов p1 и p2
    if len(p1.terminals) != len(p2.terminals):
        # Если длины не равны, то унификация невозможна
        print('Длины не совпадают')
        return False

    original = copy.deepcopy(table)
    # Перебирает пары термов t1 и t2 из списков термов атомов p1 и p2, используя функцию zip, которая сопоставляет элементы с одинаковыми индексами.
    for t1, t2 in zip(p1.terminals, p2.terminals):
        # Проверяем является ли терм t1 переменной (проверка флага)
        if t1.variable:
            # Если первый терм переменная, проверяем является ли второй терм переменной (проверка флага)
            if t2.variable:
                y = True  # флаг возможности унификации

                # Проверяем есть ли обе переменные в таблице подстановок
                if t1.name not in table.variables and t2.name not in table.variables:
                    # Если их нет, добавляем и связываем
                    table.variables[t1.name] = t2.name
                    table.variables[t2.name] = t1.name

                # Проверяем есть ли первая переменная в таблице подстановок
                elif t1.name not in table.variables:
                    # Переменные равны и связаны между собой
                    table.variables[t1.name] = table.variables[t2.name]

                # Проверяем есть ли вторая переменная в таблице подстановок
                elif t2.name not in table.variables:
                    # Переменные равны и связаны между собой
                    table.variables[t1.name] = table.variables[t2.name]

                # Проверяем равенство значений переменных в таблице подстановок
                elif table.variables[t1.name] != table.variables[t2.name]:
                    # Если значения не равны, то меняем флаг унификации (она невозможна)
                    y = False

                if y == False:
                    print("Переменная ", t1.name, " не соответствует другой переменной ", t2.name, ": ", table.val(t1),
                          " != ", table.val(t2), sep='')
                    table.reset(original)
                    return False

            # Если первый терм переменная, а второй терм константа
            else:
                y = True
                # Проверяем, есть ли переменная t1 в таблице подстановок и есть ли у нее значение (константа)
                if t1.name in table.variables and type(table.variables[t1.name]) is not str:
                    # проверяем, равно ли значение переменной t1 в таблице подстановок значению константы t2
                    if table.variables[t1.name].value != t2.value:
                        # если не равно, то меняем флаг унификации, она невозможна
                        y = False

                # Если переменной нет в таблцие подстановок
                if t1.name not in table.variables:
                    # присваиванем значению переменной t1 в таблице подстановок значения константы t2.
                    table.variables[t1.name] = t2

                # Если у переменной нет значения (константы)
                if type(table.variables[t1.name]) is str:
                    # присваиванем значению переменной t1 в таблице подстановок значения константы t2. и всем связанным переменным тоже
                    k = table.variables[t1.name]
                    table.variables[t1.name] = t2
                    table.variables[k] = t2
                    table.links[t2.value] = {k}

                # проверяем, есть ли константа t2 в таблице связей
                if t2.value not in table.links:
                    # Связываем переменную с этой константой.
                    table.links[t2.value] = {t1.name}
                else:
                    # если константы нет в таблице связей, то дабвляем эту связь
                    table.links[t2.value].add(t1.name)

                if y == False:
                    print("Несоответствующее значение переменной константе: ", t1.name, " = ", table.val(t1),
                          "константа", t2.value)
                    table.reset(original)
                    return False


        # Терм t1 не является переменной
        else:
            # Проверяем является ли второй терм t2 переменной (проверка флага)
            if t2.variable:
                # t2 переменная
                y = True

                # Проверяем, есть ли переменная t1 в таблице подстановок и есть ли у нее значение (константа)
                if t2.name in table.variables and type(table.variables[t2.name]) is not str:
                    # проверяем, равно ли значение переменной t1 в таблице подстановок значению константы t2
                    if table.variables[t2.name].value != t1.value:
                        # если не равно, то меняем флаг унификации, она невозможна
                        y = False

                # Если переменной нет в таблцие подстановок
                if t2.name not in table.variables:
                    # присваиванем значению переменной t1 в таблице подстановок значения константы t2.
                    table.variables[t2.name] = t1

                # Если у переменной нет значения (константы)
                if type(table.variables[t2.name]) is str:
                    # присваиванем значению переменной t1 в таблице подстановок значения константы t2. и всем связанным переменным тоже
                    k = table.variables[t2.name]
                    table.variables[t2.name] = t1
                    table.variables[k] = t1
                    table.links[t1.value] = {k}

                # проверяет, есть ли константа t1 в таблице связей
                if t1.value not in table.links:
                    # Связываем переменную с этой константой.
                    table.links[t1.value] = {t2.name}
                else:
                    # если константы нет в таблице связей, то дабвляем эту связь
                    table.links[t1.value].add(t2.name)

                if y == False:
                    print("Несоответствующее значение переменной константы:", t2.name, "=", table.val(t2),
                          "константа", t1.value)
                    table.reset(original)
                    return False
            else:
                # t2 не перменная, сравниваем начения двух констант
                if t1.value != t2.value:
                    print("Константы не соответствуют:", t1.value, "!=", t2.value)
                    table.reset(original)
                    return False
    return True


class HyperGraphSearcher:
    def __init__(self, rules):
        self.rules = rules
        self.table = None

        # Списки доказанных атомов и правил
        self.proven_atoms = list()
        self.proven_rules = list()

        # Списки открытых атомов и правил
        self.opened_atoms = list()
        self.opened_rules = list()
        self.used_atoms = list()

        self.found = False  # флаг решения

    def search_from_target(self, input_atoms, target_atom):
        self.table = Table()

        # Добавляем целевой атом в список открытых атомов
        self.opened_atoms.insert(0, target_atom)
        # Заносим входные данные (атомы) в список доказанных атомов
        self.proven_atoms = list(input_atoms)

        # Подцель берем из головы И потомки записываются в голову, механзм СТЕКА
        current = self.opened_atoms[0]

        # Пока не найдено решение или не закончились атомы (потомки)
        while not self.found and len(self.opened_atoms) != 0:
            print('\nТекущая подцель', current)

            print(f"Список доказанных атомов: ", self.proven_atoms)
            print(f"Список доказанных правил: ", self.proven_rules)

            end_vertex = True  # Флаг все атомы доказаны
            prove_num = False  # Флаг доказанного номера правила
            check_unif = True  # Флаг нужно провести унификацию

            # Пока не конец базы правил
            for num, rule in self.rules.items():

                # Метод потомков, ищем правило в базе правил, выходной атом которого унфицируется с подцелью
                if check_unif:
                    if unification(self.table, rule.output_vertex, current):
                        print('Номер текущего правила: ', num)
                        print("Унификация выполнена:", rule.output_vertex, current)
                        print('Tаблица подстановок:', self.table.variables)
                        print('Таблица cвязей:', self.table.links)
                        check_unif = False
                        self.used_atoms.insert(0, current)

                if not check_unif:
                    # Смотрим входные атомы для текущего правила
                    for node in rule.input_atoms:
                        # Проверяем входят ли входные атомы в список закрытых атомов
                        if node not in self.used_atoms:
                            print('\nАтом', node)
                            end_vertex = False
                            prove_num = False

                            # Если номер текущего правила не в списке открытых, добавляем номер правила в список открытых
                            if num not in self.opened_rules:
                                self.opened_rules.insert(0, num)

                            # Пока не конец базы фактов
                            for proven in self.proven_atoms:
                                if unification(self.table, node, proven):
                                    print("Атом", node, "уже доказан")
                                    print('Tаблица подстановок:', self.table.variables)
                                    print('Таблица cвязей:', self.table.links)

                                    prove_num = True
                                    self.used_atoms.insert(0, node)
                                    break

                            if prove_num == False:
                                # Если атом текущего правила не в списке открытых, добавляем атом в список открытых
                                if node not in self.opened_atoms and node not in self.used_atoms:
                                    self.opened_atoms.insert(0, node)

                                # Меняем подцель, берем из головы
                                current = self.opened_atoms[0]
                                print('Атом становится новой подцелью')
                                check_unif = True
                                # Переходим к раскрытию новой подцели
                                break

                    # Если все атомы в списке фактов (закрытых) -> разметка
                    if prove_num:
                        # Распространение
                        for i in range(len(current.terminals)):
                            current.terminals[i] = self.table.variables[str(current.terminals[i])]

                        if len(self.opened_atoms) != 0:
                            # Добавляем в список фактов (закрытых), убираем подцель из стека
                            self.proven_atoms.append(self.opened_atoms.pop(0))
                            if len(self.opened_rules) != 0:
                                self.proven_rules.append(self.opened_rules.pop(0))

                        print(f"Список доказанных атомов: ", self.proven_atoms)
                        print(f"Список доказанных правил: ", self.proven_rules)

                        # Выбираем следующий атом (подцель) из головы
                        if len(self.opened_atoms) != 0 and len(self.opened_rules) != 0:
                            current = self.opened_atoms[0]

                        # Список открытых атомов пуст
                        else:
                            # Проверяем есть ли целевой атом в списке доказанных
                            if target_atom in self.proven_atoms:
                                print('Целевой атом в списке доказанных!')
                                self.found = True  # меняем флаг решения
                                break
                            else:
                                print("Список открытых атомов пуст!")
                                break
                        break

            # Если все атомы в правиле доказаны
            if end_vertex:
                print('Доказали все атомы в правиле, правило доказано!')

                if len(self.opened_atoms) != 0:
                    # Добавляем эту вершину в список доказанных
                    self.proven_atoms.append(self.opened_atoms.pop(0))

                    if len(self.opened_rules) != 0:
                        # Добавляем номер правила в список доказанных
                        self.proven_rules.append(self.opened_rules.pop(0))

                print('\nКонечные списки')
                print(f"Список доказанных атомов: ", self.proven_atoms)
                print(f"Список доказанных правил: ", self.proven_rules)

                if len(self.opened_atoms) != 0:
                    # Меняем подцель
                    current = self.opened_atoms[0]
                else:
                    self.found = True

        if self.found:
            print('\nРешение найдено!')
            return self.found, self.proven_atoms, self.proven_rules
        else:
            if len(self.opened_atoms) == 0:
                print('\nАтом НЕ найден! Список открытых атомов пуст!')
                return self.found


def main():
    c_N = Constant('N')
    c_M1 = Constant('M1')
    c_W = Constant('W')
    c_A1 = Constant('A1')

    v_x = Variable("x")
    v_y = Variable("y")
    v_z = Variable("z")
    v_x1 = Variable("x1")
    v_x2 = Variable("x2")
    v_x3 = Variable("x3")

    node1 = Atom("A", [v_x])
    node2 = Atom("W", [v_y])
    node3 = Atom("S", [v_x, v_y, v_z])
    node4 = Atom("H", [v_z])
    node5 = Atom("C", [v_x])

    node6 = Atom("M", [v_x1])
    node7 = Atom("O", [c_N, v_x1])
    node8 = Atom("S", [c_W, v_x1, c_N])

    node9 = Atom("M", [v_x2])
    node10 = Atom("W", [v_x2])

    node11 = Atom("E", [v_x3, c_A1])
    node12 = Atom("H", [v_x3])

    rules = dict()
    rules[1] = Rule([node1, node2, node3, node4], node5)
    rules[2] = Rule([node6, node7], node8)
    rules[3] = Rule([node9], node10)
    rules[4] = Rule([node11], node12)

    graph = HyperGraphSearcher(rules)

    target = Atom("C", [c_W])
    given = [
        Atom("O", [c_N, c_M1]),
        Atom("M", [c_M1]),
        Atom("A", [c_W]),
        Atom("E", [c_N, c_A1]),
    ]

    res, nodes, rules = graph.search_from_target(given, target)

    print("\nДоказанные атомы:", nodes)
    print("Доказанные правила:", rules)
    print("Таблица подстановок:", graph.table, sep='\n')


if __name__ == "__main__":
    main()
