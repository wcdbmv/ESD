#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Term {
  std::string name;
  bool is_const;

  static Term const_(std::string name) { return {name, true}; }
  static Term var(std::string name) { return {name, false}; }

  bool operator==(Term const& oth) const = default;
  std::string to_str() const { return name; }
};

struct Atom {  // Predicate
  std::string name;
  bool negative;
  std::vector<Term> args;

  bool operator==(Atom const& oth) const = default;

  std::string to_str() const {
    std::string res = (negative ? "!" : "") + name + "(";
    bool first = true;
    for (Term const& t : args) {
      res += (first ? "" : ", ") + t.to_str();
      first = false;
    }
    return res + ")";
  }
};

struct Disjunct {
  std::vector<Atom> atoms;
  Disjunct() : atoms() {}
  Disjunct(std::initializer_list<Atom> list) : atoms(list) {}

  bool operator==(Disjunct const& oth) const = default;

  std::string to_str() const {
    if (atoms.empty())
      return "<empty>";
    std::string res = "";
    bool first = true;
    for (Atom const& a : atoms) {
      res += (first ? "" : " | ") + a.to_str();
      first = false;
    }
    return res;
  }
};

struct Formula {
  std::vector<Disjunct> items;
  Formula(std::initializer_list<Disjunct> list) : items(list) {}
};

class ResolutionSolver {
 private:
  std::vector<Disjunct> disjuncts;
  bool final_result = false;
  bool iter_changed;

  // Заменить все вхождения переменной old_v на переменную или константу nev_v
  void substitute_terms(std::string old_v, std::string new_v, bool make_const) {
    std::cout << "  Замена: " << old_v << " -> " << new_v
              << (make_const ? " const" : "") << std::endl;
    for (Disjunct& d : disjuncts) {
      for (Atom& atom : d.atoms) {
        for (Term& term : atom.args) {
          if (term.name == old_v) {
            assert(!term.is_const);
            term.name = new_v;
            if (make_const)
              term.is_const = true;
          }
        }
      }
    }
  }

  // Сформировать дизъюнк из base путём исключения атома с индексом out_idx
  std::pair<Disjunct, bool> get_new_disjunct(Disjunct& base, size_t out_idx) {
    Disjunct new_disjunct;
    for (size_t j = 0; j < base.atoms.size(); j++) {
      if (j != out_idx)
        new_disjunct.atoms.push_back(base.atoms[j]);
    }

    bool present =
        std::any_of(disjuncts.begin(), disjuncts.end(),
                    [&](Disjunct const& d) { return new_disjunct == d; });

    return {new_disjunct, present};
  }

  // Есть ли заданный дизъюнкт в списке
  bool new_disjunct_present(Disjunct& base, size_t out_idx) {
    return get_new_disjunct(base, out_idx).second;
  }

  // Добавить дизъюнкт в список
  bool add_new_disjunct(Disjunct& base, size_t out_idx) {
    auto [disj, present] = get_new_disjunct(base, out_idx);
    if (present)
      return false;

    std::cout << "  " << disj.to_str() << std::endl;
    if (disj.atoms.size() == 0)
      final_result = true;

    disjuncts.push_back(disj);
    return true;
  }

  // Попытаться унифицировать 2 атома, возвращает true, если унифицировано
  bool unify_atoms(Atom& a1, Atom& a2) {
    assert(a1.name == a2.name);
    assert(a1.args.size() == a2.args.size());
    assert(a1.negative != a2.negative);

    // Предварительные списки замен
    std::vector<std::pair<std::string, std::string>> consts_mappings;
    std::vector<std::pair<std::string, std::string>> linked_vars;

    // Сопоставление аргументов предикатов
    for (size_t i = 0; i < a1.args.size(); i++) {
      Term const* arg1 = &a1.args[i];
      Term const* arg2 = &a2.args[i];

      if (arg1->is_const && arg2->is_const) {  // Обе константы
        if (arg1->name != arg2->name)
          return false;
      } else if (!arg1->is_const && !arg2->is_const) {  // Обе переменные
        if (arg1->name != arg2->name)
          linked_vars.emplace_back(arg1->name, arg2->name);
      } else {  // Константа и переменная
        if (arg1->is_const)
          std::swap(arg1, arg2);  // arg1 - var, arg2 - const
        consts_mappings.emplace_back(arg1->name, arg2->name);
      }
    }

    // Объединение связанных переменных
    static int counter = 1;
    std::map<std::string, int> new_vars;
    for (auto const& [var1, var2] : linked_vars) {
      if (new_vars.contains(var1) && new_vars.contains(var2)) {
        int num1 = new_vars.at(var1);
        int num2 = new_vars.at(var2);
        for (auto& [var, num] : new_vars) {
          if (num == num2)
            num = num1;
        }
      } else {
        if (new_vars.contains(var1)) {
          new_vars.emplace(var2, new_vars.at(var1));
        } else if (new_vars.contains(var2)) {
          new_vars.emplace(var1, new_vars.at(var2));
        } else {
          int new_num = counter++;
          new_vars.emplace(var1, new_num);
          new_vars.emplace(var2, new_num);
        }
      }
    }

    // Применение связанных переменных к списку замен констант
    for (auto const& [var, num] : new_vars) {
      for (auto& [old_v, new_v] : consts_mappings) {
        if (old_v == var)
          old_v = "@" + std::to_string(num);
      }
    }

    // Проверка замен констант на возможность унификации (нет двух разных замен
    // одной переменной)
    std::map<std::string, std::string> vars_vals;
    for (auto& [old_v, new_v] : consts_mappings) {
      if (vars_vals.contains(old_v)) {
        if (vars_vals.at(old_v) != new_v)
          return false;
      } else {
        vars_vals.emplace(old_v, new_v);
      }
    }

    // Замена связанных переменных
    for (auto const& [var, num] : new_vars) {
      std::string new_name = "@" + std::to_string(num);
      substitute_terms(var, new_name, false);
    }
    // Замена констант
    for (auto const& [old_v, new_v] : vars_vals) {
      substitute_terms(old_v, new_v, true);
    }
    return true;
  }

  // Проверка пары дизъюнктов на наличие контрактной пары, и возможная
  // унификация
  bool check_disjuncts(Disjunct& d1, Disjunct& d2) {
    for (size_t i1 = 0; i1 < d1.atoms.size(); i1++) {
      for (size_t i2 = 0; i2 < d2.atoms.size(); i2++) {
        Atom& atom1 = d1.atoms[i1];
        Atom& atom2 = d2.atoms[i2];

        if (!(atom1.name == atom2.name && atom1.negative != atom2.negative))
          continue;
        if (new_disjunct_present(d1, i1) && new_disjunct_present(d2, i2))
          continue;

        std::cout << "Унификация: " << atom1.to_str() << " И " << atom2.to_str()
                  << std::endl;
        bool unified = unify_atoms(atom1, atom2);
        if (!unified) {
          std::cout << "  Невозможна" << std::endl;
          continue;
        }

        std::cout << "Новые:\n";
        add_new_disjunct(d1, i1);
        add_new_disjunct(d2, i2);
        return true;
      }
    }
    return false;
  }

  void print_disjuncts() {
    std::cout << "Дизъюнкты:\n";
    for (Disjunct const& d : disjuncts)
      std::cout << "  " << d.to_str() << "\n";
    std::cout << "=======================================" << std::endl;
  }

 public:
  ResolutionSolver(std::vector<Formula> const& formulas,
                   Formula const& neg_target) {
    for (Formula const& f : formulas) {
      for (Disjunct const& d : f.items) {
        disjuncts.push_back(d);
      }
    }
    for (Disjunct const& d : neg_target.items) {
      disjuncts.push_back(d);
    }
  };

  void solve() {
    print_disjuncts();

    iter_changed = true;
    while (iter_changed) {
      iter_changed = false;

      for (Disjunct& d1 : disjuncts) {
        for (Disjunct& d2 : disjuncts) {
          if (&d1 == &d2)
            continue;

          bool match = check_disjuncts(d1, d2);
          if (match) {
            iter_changed = true;
            break;
          }
        }
        if (iter_changed)
          break;
      }

      print_disjuncts();
      if (final_result)
        break;
    }

    if (final_result)
      std::cout << "Доказано" << std::endl;
    else
      std::cout << "He доказано" << std::endl;
  }
};

static const bool NEG = true;
static const bool POS = false;

int main() {
  // P2(x1, y1) | P5(w1) | !P6(z1)
  // P3(C) | !P4(z1) | P1(x1, y1, z1)
  Formula f2_1{
      Disjunct{
          Atom{"P2", POS, {Term::var("x1"), Term::var("y1")}},
          Atom{"P5", POS, {Term::var("w1")}},
          Atom{"P6", NEG, {Term::var("z1")}},
      },
      Disjunct{
          Atom{"P3", POS, {Term::const_("C")}},
          Atom{"P4", NEG, {Term::var("z1")}},
          Atom{"P1", POS, {Term::var("x1"), Term::var("y1"), Term::var("z1")}},
      },
  };

  // !P2(A, B) | P5(w2) | P6(x2)
  // P4(z2) | !P3(x2)
    Formula f2_2 {
        Disjunct {
            Atom { "P2", NEG, { Term::const_("A"), Term::const_("B") } },
            Atom { "P5", POS, { Term::var("w2") } },
            Atom { "P6", POS, { Term::var("z2") } },
        },
        Disjunct {
            Atom { "P4", POS, { Term::var("z2") } },
            Atom { "P3", NEG, { Term::var("z2") } },
        },
    };

    // _ !P1(A, B, C)
    Formula neg_target {
        Disjunct {
            Atom { "P1", NEG, { Term::const_("A"), Term::const_("B"), Term::const_("C") } },
        },
    };

    ResolutionSolver({ f2_1, f2_2 }, neg_target).solve();
    return 0;
}