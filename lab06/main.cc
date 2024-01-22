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
  std::vector<Clause> clauses_;

  /// Заменить все вхождения переменной old на переменную или константу new.
  void SubstituteTerminals(const std::string& old_name,
                           const std::string& new_name,
                           const bool do_make_const) {
    std::cout << "  Замена: " << old_name << " -> " << new_name
              << (do_make_const ? " const" : "") << '\n';
    for (auto& clause : clauses_) {
      for (auto& atom : clause.atoms()) {
        for (auto& terminal : atom.terminals()) {
          if (terminal.name() != old_name) {
            continue;
          }

          assert(!terminal.IsConstant());
          terminal.set_name(new_name);
          if (do_make_const) {
            terminal.MakeConstant();
          }
        }
      }
    }
  }

  /// Сформировать дизъюнк из base путём исключения атома с индексом i_without.
  std::pair<Clause, bool> GetNewClause(const Clause& base,
                                       const size_t i_without) {
    const auto& base_atoms = base.atoms();
    assert(!base_atoms.empty());
    assert(base_atoms.size() > i_without);

    Clause new_clause;
    auto& new_atoms = new_clause.atoms();
    new_atoms.reserve(base_atoms.size() - 1);

    for (size_t i = 0; i < base_atoms.size(); ++i) {
      if (i != i_without) {
        new_atoms.push_back(base_atoms[i]);
      }
    }

    const auto is_present = std::ranges::any_of(
        clauses_,
        [&new_clause](const Clause& clause) { return clause == new_clause; });

    return {std::move(new_clause), is_present};
  }

  /// Есть ли заданный дизъюнкт в списке.
  bool NewClausePresent(const Clause& base, const size_t i_without) {
    return GetNewClause(base, i_without).second;
  }

  /// Добавить дизъюнкт в список.
  bool AddNewClause(const Clause& base,
                    const size_t i_without,
                    bool& is_final_result) {
    auto [clause, is_present] = GetNewClause(base, i_without);
    if (is_present) {
      return false;
    }

    std::cout << "  " << clause << '\n';
    if (clause.atoms().empty()) {
      is_final_result = true;
    }

    clauses_.push_back(std::move(clause));
    return true;
  }

  /// Попытаться унифицировать 2 атома, возвращает true, если унифицировано.
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  bool UnifyAtoms(const Atom& a1, const Atom& a2) {
    assert(a1.name() == a2.name());
    assert(a1.terminals().size() == a2.terminals().size());
    assert(a1.is_negative() != a2.is_negative());

    // Предварительные списки замен.
    using Mapping = std::vector<std::pair<std::string, std::string>>;
    Mapping consts_mappings;
    Mapping linked_vars;

    // Сопоставление аргументов предикатов.
    for (size_t i = 0; i < a1.terminals().size(); ++i) {
      const auto* t1 = &a1.terminals()[i];
      const auto* t2 = &a2.terminals()[i];

      if (t1->IsConstant() && t2->IsConstant()) {  // Обе константы.
        if (t1->name() != t2->name()) {
          return false;
        }
      } else if (t1->IsVariable() && t2->IsVariable()) {  // Обе переменные.
        if (t1->name() != t2->name()) {
          linked_vars.emplace_back(t1->name(), t2->name());
        }
      } else {  // Константа и переменная
        if (t1->IsConstant()) {
          std::swap(t1, t2);  // t1 - var, t2 - const
        }
        consts_mappings.emplace_back(t1->name(), t2->name());
      }
    }

    // Объединение связанных переменных.
    static int counter = 1;
    std::map<std::string, int> new_vars;
    for (auto& [var1, var2] : linked_vars) {
      auto it1 = new_vars.find(var1);
      auto it2 = new_vars.find(var2);
      if (it1 != new_vars.end() && it2 != new_vars.end()) {
        const int num1 = it1->second;
        const int num2 = it2->second;
        for (auto& [var, num] : new_vars) {
          if (num == num2) {
            num = num1;
          }
        }
      } else if (it1 != new_vars.end()) {
        new_vars.emplace(std::move(var2), it1->second);
      } else if (it2 != new_vars.end()) {
        new_vars.emplace(std::move(var1), it2->second);
      } else {
        const int new_num = counter++;
        new_vars.emplace(std::move(var1), new_num);
        new_vars.emplace(std::move(var2), new_num);
      }
    }

    // Применение связанных переменных к списку замен констант.
    for (const auto& [var, num] : new_vars) {
      for (auto& [old_v, new_v] : consts_mappings) {
        if (old_v == var) {
          old_v = "@" + std::to_string(num);
        }
      }
    }

    // Проверка замен констант на возможность унификации (нет двух разных замен
    // одной переменной).
    std::map<std::string, std::string> vars_vals;
    for (auto& [old_v, new_v] : consts_mappings) {
      if (auto it = vars_vals.find(old_v); it != vars_vals.end()) {
        if (it->second != new_v) {
          return false;
        }
      } else {
        vars_vals.emplace(std::move(old_v), std::move(new_v));
      }
    }

    // Замена связанных переменных.
    for (auto const& [var, num] : new_vars) {
      const std::string new_name = "@" + std::to_string(num);
      SubstituteTerminals(var, new_name, false);
    }
    // Замена констант.
    for (auto const& [old_v, new_v] : vars_vals) {
      SubstituteTerminals(old_v, new_v, true);
    }
    return true;
  }

  // Проверка пары дизъюнктов на наличие контрарной пары, и возможная
  // унификация.
  bool CheckClauses(const Clause& c1, const Clause& c2, bool& is_final_result) {
    const auto& a1 = c1.atoms();
    const auto& a2 = c2.atoms();

    for (size_t i = 0; i < a1.size(); ++i) {
      for (size_t j = 0; j < a2.size(); ++j) {
        const auto& atom1 = a1[i];
        const auto& atom2 = a2[j];

        if (atom1.name() != atom2.name() ||
            atom1.is_negative() == atom2.is_negative()) {
          continue;
        }

        if (NewClausePresent(c1, i) && NewClausePresent(c2, j)) {
          continue;
        }

        std::cout << "Унификация: " << atom1 << " И " << atom2 << '\n';
        if (!UnifyAtoms(atom1, atom2)) {
          std::cout << "  Невозможна" << '\n';
          continue;
        }

        std::cout << "Новые:\n";
        AddNewClause(c1, i, is_final_result);
        AddNewClause(c2, j, is_final_result);
        return true;
      }
    }
    return false;
  }

  void PrintClauses() const {
    std::cout << "Дизъюнкты:\n";
    for (Clause const& d : clauses_) {
      std::cout << "  " << d << "\n";
    }
    std::cout << "=======================================" << '\n';
  }

 public:
  ResolutionSolver(const std::vector<Formula>& formulas,
                   const Formula& neg_target) {
    for (const auto& formula : formulas) {
      for (const auto& clause : formula.clauses()) {
        clauses_.push_back(clause);
      }
    }
    for (const auto& clause : neg_target.clauses()) {
      clauses_.push_back(clause);
    }
  }

  void Solve() {
    auto is_final_result = false;
    auto is_iter_changed = true;
    while (is_iter_changed && !is_final_result) {
      PrintClauses();
      is_iter_changed = false;

      for (Clause& c1 : clauses_) {
        for (Clause& c2 : clauses_) {
          if (&c1 == &c2) {
            continue;
          }

          if (CheckClauses(c1, c2, is_final_result)) {
            is_iter_changed = true;
            break;
          }
        }
        if (is_iter_changed) {
          break;
        }
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

    // P2(x1, y1) | P5(w1) | ¬P6(z1)
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

    // ¬P2(A, B) | P5(w2) | P6(x2)
    // P4(z2) | ¬P3(x2)
    const Formula f2_2{
        Clause{
            Atom{"P2",
                 {Terminal::Constant("A"), Terminal::Constant("B")},
                 kNegative},
            Atom{"P5", {Terminal::Variable("w2")}},
            Atom{"P6", {Terminal::Variable("z2")}},
        },
        Clause {
            Atom{"P4", {Terminal::Variable("z2")}},
            Atom{"P3", {Terminal::Variable("z2")}, kNegative},
        },
    };

    // ¬P1(A, B, C)
    const Formula neg_target{
        Clause{
            Atom{"P1",
                 {Terminal::Constant("A"), Terminal::Constant("B"),
                  Terminal::Constant("C")},
                 kNegative},
      },
    };

    ResolutionSolver({f2_1, f2_2}, neg_target).Solve();
    return 0;
  }
  catch (const std::exception& exception) {
    std::cout << "[exception] " << exception.what() << '\n';
    return 1;
  }
}
