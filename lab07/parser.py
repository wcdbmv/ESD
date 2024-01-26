import pyparsing as pp
from models import *

pp.ParserElement.enablePackrat()


def create_operation(t):
    opTypes = set(t[0][1::2])
    assert len(opTypes) == 1, "Invalid formula"
    if '→' in opTypes:
        assert len(t[0][0::2]) == 2, "Invalid formula"
    return Operation(sym2type(t[0][1]), t[0][0::2])


# Операнды
variable = pp.Regex(r'[a-zA-Zа-яА-ЯёЁ0-9]+').setResultsName('variable')
variable = variable.setParseAction(lambda t: Variable(t[0]))

arguments = pp.Group(pp.Suppress('(') + pp.delimitedList(variable) + pp.Suppress(')')).setResultsName('arguments')

predicate = pp.Regex(r'[a-zA-Zа-яА-ЯёЁ][a-zA-Zа-яА-ЯёЁ0-9]*') + arguments
predicate = predicate.setResultsName('predicate')
predicate = predicate.setParseAction(lambda t: Predicate(t[0], t.arguments.asList()))

operand = predicate | variable

formula = pp.Forward()
quantifiers = pp.oneOf("∃ ∀") + variable + formula
quantifiers.setParseAction(lambda t:
                           Quantifier(sym2type(t[0]), t[1], t[2]))

expr = pp.infixNotation(operand | quantifiers, [
    ("¬", 1, pp.opAssoc.RIGHT, lambda t: Operation(OpType.NOT, [t[0][1]])),
    (pp.oneOf("& | → ="), 2, pp.opAssoc.LEFT, create_operation),
])
formula <<= expr


# formula = pp.infix_notation(operand, [
#     (pp.one_of("∃ ∀"), 1, pp.opAssoc.RIGHT, lambda t: Quantifier(sym2type(t[0][0]), t[0][1])),
#     ("¬", 1, pp.opAssoc.RIGHT, lambda t: Operation(OpType.NOT, [t[0][1]])),
#     (pp.one_of("& | → ="), 2, pp.opAssoc.LEFT, lambda t: Operation(sym2type(t[0][1]), t[0][0::2])),
# ])


def main():
    res = formula.parseString('(p2(x) & p3(x) & p4(x)) → p5(x)')
    print(res[0])

    res = formula.parseString('p1(Pepe)')
    print(res[0])

    res = formula.parseString('¬(¬x)')
    print(res[0])

    res = formula.parseString('∀x (x | ¬x)')
    print(res[0])

    res = formula.parseString('p1(x, y)')
    print(res[0])

    res = formula.parseString('¬(x | z) & (¬x → p1(x, y))')
    print(res[0])

    res = formula.parseString('((x | (x → y)) & (y | z)) & (x = z)')
    print(res[0])

    res = formula.parseString('¬(x & y & z)')
    print(res[0])


if __name__ == '__main__':
    main()
