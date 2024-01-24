import copy
from parser import *
from models import *
from typing import *

def prepare(facts, rules, goal):
    facts = [r.strip().rstrip() for r in facts.split('\n') if len(r.strip().rstrip()) > 0]
    rules = [r.strip().rstrip() for r in rules.split('\n') if len(r.strip().rstrip()) > 0]

    facts_g = []
    for f in facts:
        res = formula.parseString(f)[0]
        facts_g.append(res)

    rules_g = []
    for rule_idx, r in enumerate(rules):
        rule = formula.parseString(r)[0]
        rule = Rule(rule)
        rules_g.append(rule)

    goal_g = formula.parseString(goal)[0]

    print("facts:")
    for f in facts_g:
        print('\t', f)

    print("rules:")
    for r in rules_g:
        print('\t', r)

    print("goal:")
    print("\t", goal_g)
    print('-------------------------------\n')

    return facts_g, rules_g, goal_g

class Prover:
    def __init__(self):
        self.closedFacts = []
        self.closedRules = []
        self.counter = 1

    def substitute_terms(self, rules, old_v, new_v, to_const):
        # print(f'{old_v} -> {new_v}')
        for rule in rules:
            if rule is not None:
                rule.rename_var(old_v, new_v, to_const)

    def tryUnify(self, a1, a2, rule):
        if a1.name != a2.name or len(a1.args) != len(a2.args):
            return False

        consts_mappings = []
        linked_vars = []

        for arg1, arg2 in zip(a1.args, a2.args):
            if arg1.is_const and arg2.is_const:
                if arg1.name != arg2.name:
                    return False
            elif not arg1.is_const and not arg2.is_const:
                if arg1.name != arg2.name:
                    linked_vars.append([arg1.name, arg2.name])
            else:
                if arg1.is_const:
                    arg1, arg2 = arg2, arg1
                consts_mappings.append([arg1.name, arg2.name])

        # Объединение связанных переменных
        new_vars = dict()
        for var1, var2 in linked_vars:
            if var1 in new_vars and var2 in new_vars:
                num1 = new_vars.get(var1)
                num2 = new_vars.get(var2)
                for var in new_vars:
                    num = new_vars[var]
                    if num == num2:
                        num = num1

            else:
                if var1 in new_vars:
                    new_vars[var2] = new_vars.get(var1)
                elif var2 in new_vars:
                    new_vars[var1] = new_vars.get(var2)
                else:
                    self.counter += 1
                    new_vars[var1] = self.counter
                    new_vars[var2] = self.counter

        for var in new_vars:
            num = new_vars[var]
            for i in range(len(consts_mappings)):
                old_v, new_v = consts_mappings[i]
                if old_v == var:
                    consts_mappings[i][0] = f'@{num}'

        vars_vals = dict()
        for old_v, new_v in consts_mappings:
            if old_v in vars_vals:
                if vars_vals.get(old_v) != new_v:
                    return False
            else:
                vars_vals[old_v] = new_v

        # Замена связанных переменных
        for var in new_vars:
            num = new_vars[var]
            new_name = f"@{num}"
            self.substitute_terms([rule], var, new_name, False)
        for old_v in vars_vals:
            new_v = vars_vals[old_v]
            self.substitute_terms([rule], old_v, new_v, True)

        return True
    
    def isRuleApproved(self, rule: Rule):
        for term in rule.in_terms:
            if term not in self.closedFacts:
                return False
        return True

    def searchRules(self):
        canFound = False
        for idx, rule in enumerate(self.rules):
            if not rule.approved and rule not in self.closedRules:
                wasFound = False
                tmp_rule = copy.deepcopy(rule)
                print(f"Try approve rule: {rule}")
                for i, term in enumerate(rule.in_terms):
                    term = tmp_rule.in_terms[i]
                    if term in self.closedFacts:
                        print(f"\tTerm {term} already approved")
                    else:
                        closedFactsLen = len(self.closedFacts)
                        for k in range(closedFactsLen):
                            closedFact = self.closedFacts[k]
                            if self.tryUnify(closedFact, term, tmp_rule):
                                if tmp_rule not in self.rules:
                                    print(f"\tAdd rule: {tmp_rule}")
                                    self.rules.append(tmp_rule)
                                    wasFound = True

                                if term not in self.closedFacts:
                                    print(f"\tAdd closed fact: {term}")
                                    self.closedFacts.append(term)
                                    wasFound = True

                                tmp_rule = copy.deepcopy(rule)
                                term = tmp_rule.in_terms[i]
                canFound = canFound or wasFound
                if self.isRuleApproved(rule):
                    print(f"\tRule: {rule} approved")
                    rule.approved = True
                    self.closedRules.append(rule)
                    self.closedFacts.append(rule.out_term)
                    print(f"\tAdd closed fact: {rule.out_term}")
                    canFound = True
                # elif wasFound:
                #     self.closedRules.append(rule)

        return canFound

    def prove(self, facts: List[Predicate], rules: List[Rule], goal: Predicate):
        self.facts = facts
        self.rules = rules

        self.closedFacts = facts
        canFound = True

        while canFound and goal not in self.closedFacts:
            canFound = self.searchRules()

            print("\nclosedFacts:")
            for closedFact in self.closedFacts:
                print("\t", closedFact)
            print()
            print("-------------------------------")

        return self.getMatched(goal)

    def getMatched(self, goal):
        res = []
        tmp = deepcopy(goal)
        for closedFact in self.closedFacts:
            if self.tryUnify(closedFact, tmp, None):
                tmp = deepcopy(goal)
                res.append(closedFact)
        return res




def example2():
    facts = '''
        man(Adam)
        man(Herasim)
        man(Wallie)
        man(Pup)
        woman(Mumu)
        woman(Eva)
        
        child(Herasim, Mumu, Pup)
        child(Adam, Eva, Wallie)
        '''

    rules = '''
        (man(x) & child(x, y, z)) → father(x, z)
        (man(x) & child(y, x, z)) → father(x, z)
        (woman(x) & child(y, x, z)) → mother(x, z)
        (woman(x) & child(x, y, z)) → mother(x, z)
        '''

    #goal = 'man(Herasim)'
    goal = 'mother(Eva, Wallie)'
    return facts, rules, goal

def example3():
    facts = '''
        father(Jack, Susan)
        father(Jack, Ray)
        father(David, Liza)
        father(David, John)
        father(John, Peter)
        father(John, Mary)
        mother(Karen, Susan)
        mother(Karen, Ray)
        mother(Amy, Liza)
        mother(Amy, John)
        mother(Susan, Peter)
        mother(Susan, Mary)
        '''

    rules = '''
        father(x, y) → parent(x, y)
        mother(x, y) → parent(x, y)

        (father(x, z) & parent(z, y)) → grandfather(x, y)
        (mother(x, z) & parent(z, y)) → grandmother(x, y)
        (parent(x, z) & parent(z, y)) → grandparent(x, y)
        '''

    # goal = 'grandparent(Amy, Mary)'
    goal = 'grandparent(x, y)'
    return facts, rules, goal

def example4():
    facts = '''
        p1(Pepe)
        '''

    rules = '''
        p1(x) → p2(x)
        p1(x) → p3(x))
        p1(x) → p4(x)
        (p2(x) & p3(x) & p4(x)) → p5(x)
        '''

    goal = 'p5(Other)'
    return facts, rules, goal

def example1():
    facts = '''
        p1(Pepe)
        '''

    rules = '''
        (p2(x) & p3(x) & p4(x)) → p5(x)
        p1(x) → p2(x)
        p1(x) → p3(x))
        p1(x) → p4(x)
        '''

    goal = 'p5(Pepe)'
    return facts, rules, goal

    
facts, rules, goal = example1()
facts, rules, goal = prepare(facts, rules, goal)

p = Prover()
matched = p.prove(facts, rules, goal)
if not matched:
    print('No approved')
else:
    print('Result:')
    for match in matched:
        print("\t", match)
