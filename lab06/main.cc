#include <algorithm>
#include <cassert>
#include <cstddef>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "types/atom.h"
#include "types/clause.h"
#include "types/formula.h"
#include "types/terminal.h"

class ResolutionSolver {
 private:
  std::vector<Clause> clauses;

  // Заменить все вхождения переменной old_v на переменную или константу nev_v
  void SubstituteTerminals(const std::string& old_v,
                           const std::string& new_v,
                           bool make_const) {
    std::cout << "  Замена: " << old_v << " -> " << new_v
              << (make_const ? " const" : "") << '\n';
    for (Clause& d : clauses) {
      for (Atom& atom : d.atoms()) {
        for (Terminal& term : atom.terminals()) {
          if (term.name() == old_v) {
            assert(!term.IsConstant());
            term.set_name(new_v);
            if (make_const) {
              term.MakeConstant();
            }
          }
        }
      }
    }
  }

  // Сформировать дизъюнк из base путём исключения атома с индексом out_idx
  std::pair<Clause, bool> GetNewClause(Clause& base, size_t out_idx) {
    Clause new_clause;
    for (size_t j = 0; j < base.atoms().size(); j++) {
      if (j != out_idx) {
        new_clause.atoms().push_back(base.atoms()[j]);
      }
    }

    const auto present =
        std::any_of(clauses.begin(), clauses.end(),
                    [&](Clause const& d) { return new_clause == d; });

    return {new_clause, present};
  }

  // Есть ли заданный дизъюнкт в списке
  bool NewClausePresent(Clause& base, size_t out_idx) {
    return GetNewClause(base, out_idx).second;
  }

  // Добавить дизъюнкт в список
  bool AddNewClause(Clause& base, size_t out_idx, bool& is_final_result) {
    auto [clause, present] = GetNewClause(base, out_idx);
    if (present) {
      return false;
    }

    std::cout << "  " << clause << '\n';
    if (clause.atoms().empty()) {
      is_final_result = true;
    }

    clauses.push_back(clause);
    return true;
  }

  // Попытаться унифицировать 2 атома, возвращает true, если унифицировано
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  bool UnifyAtoms(Atom& a1, Atom& a2) {
    assert(a1.name() == a2.name());
    assert(a1.terminals().size() == a2.terminals().size());
    assert(a1.is_negative() != a2.is_negative());

    // Предварительные списки замен
    std::vector<std::pair<std::string, std::string>> consts_mappings;
    std::vector<std::pair<std::string, std::string>> linked_vars;

    // Сопоставление аргументов предикатов
    for (size_t i = 0; i < a1.terminals().size(); i++) {
      Terminal const* arg1 = &a1.terminals()[i];
      Terminal const* arg2 = &a2.terminals()[i];

      if (arg1->IsConstant() && arg2->IsConstant()) {  // Обе константы
        if (arg1->name() != arg2->name()) {
          return false;
        }
      } else if (arg1->IsVariable() && arg2->IsVariable()) {  // Обе переменные
        if (arg1->name() != arg2->name()) {
          linked_vars.emplace_back(arg1->name(), arg2->name());
        }
      } else {  // Константа и переменная
        if (arg1->IsConstant()) {
          std::swap(arg1, arg2);  // arg1 - var, arg2 - const
        }
        consts_mappings.emplace_back(arg1->name(), arg2->name());
      }
    }

    // Объединение связанных переменных
    static int counter = 1;
    std::map<std::string, int> new_vars;
    for (auto const& [var1, var2] : linked_vars) {
      if (new_vars.contains(var1) && new_vars.contains(var2)) {
        const int num1 = new_vars.at(var1);
        const int num2 = new_vars.at(var2);
        for (auto& [var, num] : new_vars) {
          if (num == num2) {
            num = num1;
          }
        }
      } else {
        if (new_vars.contains(var1)) {
          new_vars.emplace(var2, new_vars.at(var1));
        } else if (new_vars.contains(var2)) {
          new_vars.emplace(var1, new_vars.at(var2));
        } else {
          const int new_num = counter++;
          new_vars.emplace(var1, new_num);
          new_vars.emplace(var2, new_num);
        }
      }
    }

    // Применение связанных переменных к списку замен констант
    for (auto const& [var, num] : new_vars) {
      for (auto& [old_v, new_v] : consts_mappings) {
        if (old_v == var) {
          old_v = "@" + std::to_string(num);
        }
      }
    }

    // Проверка замен констант на возможность унификации (нет двух разных замен
    // одной переменной)
    std::map<std::string, std::string> vars_vals;
    for (auto& [old_v, new_v] : consts_mappings) {
      if (vars_vals.contains(old_v)) {
        if (vars_vals.at(old_v) != new_v) {
          return false;
        }
      } else {
        vars_vals.emplace(old_v, new_v);
      }
    }

    // Замена связанных переменных
    for (auto const& [var, num] : new_vars) {
      const std::string new_name = "@" + std::to_string(num);
      SubstituteTerminals(var, new_name, false);
    }
    // Замена констант
    for (auto const& [old_v, new_v] : vars_vals) {
      SubstituteTerminals(old_v, new_v, true);
    }
    return true;
  }

  // Проверка пары дизъюнктов на наличие контрактной пары, и возможная
  // унификация
  bool CheckClauses(Clause& d1, Clause& d2, bool& is_final_result) {
    for (size_t i1 = 0; i1 < d1.atoms().size(); i1++) {
      for (size_t i2 = 0; i2 < d2.atoms().size(); i2++) {
        Atom& atom1 = d1.atoms()[i1];
        Atom& atom2 = d2.atoms()[i2];

        if (atom1.name() != atom2.name() ||
            atom1.is_negative() == atom2.is_negative()) {
          continue;
        }
        if (NewClausePresent(d1, i1) && NewClausePresent(d2, i2)) {
          continue;
        }

        std::cout << "Унификация: " << atom1 << " И " << atom2 << '\n';
        const bool unified = UnifyAtoms(atom1, atom2);
        if (!unified) {
          std::cout << "  Невозможна" << '\n';
          continue;
        }

        std::cout << "Новые:\n";
        AddNewClause(d1, i1, is_final_result);
        AddNewClause(d2, i2, is_final_result);
        return true;
      }
    }
    return false;
  }

  void PrintClauses() {
    std::cout << "Дизъюнкты:\n";
    for (Clause const& d : clauses) {
      std::cout << "  " << d << "\n";
    }
    std::cout << "=======================================" << '\n';
  }

 public:
  ResolutionSolver(std::vector<Formula> const& formulas,
                   Formula const& neg_target) {
    for (Formula const& f : formulas) {
      for (Clause const& d : f.clauses()) {
        clauses.push_back(d);
      }
    }
    for (Clause const& d : neg_target.clauses()) {
      clauses.push_back(d);
    }
  }

  void Solve() {
    auto is_final_result = false;
    auto is_iter_changed = true;
    while (is_iter_changed) {
      PrintClauses();
      is_iter_changed = false;

      for (Clause& d1 : clauses) {
        for (Clause& d2 : clauses) {
          if (&d1 == &d2) {
            continue;
          }

          const bool match = CheckClauses(d1, d2, is_final_result);
          if (match) {
            is_iter_changed = true;
            break;
          }
        }
        if (is_iter_changed) {
          break;
        }
      }

      if (is_final_result) {
        break;
      }
    }

    PrintClauses();

    if (is_iter_changed) {
      std::cout << "Доказано" << '\n';
    } else {
      std::cout << "He доказано" << '\n';
    }
  }
};

int main() {
  try {
    constexpr auto kNegative = true;

    // P2(x1, y1) | P5(w1) | !P6(z1)
    // P3(C) | !P4(z1) | P1(x1, y1, z1)
    const Formula f2_1{
        Clause{
            Atom{"P2", {Terminal::Variable("x1"), Terminal::Variable("y1")}},
            Atom{"P5", {Terminal::Variable("w1")}},
            Atom{"P6", {Terminal::Variable("z1")}, kNegative},
        },
        Clause{
            Atom{"P3", {Terminal::Constant("C")}},
            Atom{"P4", {Terminal::Variable("z1")}, kNegative},
            Atom{"P1",
                 {Terminal::Variable("x1"), Terminal::Variable("y1"),
                  Terminal::Variable("z1")}},
        },
    };

    // !P2(A, B) | P5(w2) | P6(x2)
    // P4(z2) | !P3(x2)
    const Formula f2_2{
        Clause{
            Atom{"P2",
                 {Terminal::Constant("A"), Terminal::Constant("B")},
                 kNegative},
            Atom { "P5", { Terminal::Variable("w2") } },
        Atom { "P6", { Terminal::Variable("z2") } },
    },
    Clause {
      Atom { "P4", { Terminal::Variable("z2") } },
      Atom { "P3", { Terminal::Variable("z2") }, kNegative },
      },
    };

    // _ !P1(A, B, C)
    const Formula neg_target {
      Clause {
        Atom { "P1", { Terminal::Constant("A"), Terminal::Constant("B"), Terminal::Constant("C") }, kNegative },
      },
    };

    ResolutionSolver({ f2_1, f2_2 }, neg_target).Solve();
    return 0;
  }
  catch (const std::exception& exception) {
    std::cout << "[exception] " << exception.what() << '\n';
  }
}
