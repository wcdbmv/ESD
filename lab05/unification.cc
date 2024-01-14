#include "unification.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <list>
#include <set>
#include <string_view>
#include <unordered_map>

#include "types/atom.h"
#include "types/constant.h"
#include "types/name.h"
#include "types/terminal.h"
#include "types/variable.h"
#include "utils/io_join.h"

namespace {

class Unification {
 public:
  Unification(const Atom& atom1, const Atom& atom2)
      : atom1_{atom1}, atom2_{atom2} {}

  bool Run() {
    std::cout << "[Унификация]\nАтом 1: " << atom1_ << "\nАтом 2: " << atom2_
              << '\n';

    if (atom1_.name != atom2_.name) {
      std::cout << "Имена атомов не совпали\n";
      return false;
    }

    const auto terminals_size = atom1_.terminals.size();
    if (terminals_size != atom2_.terminals.size()) {
      std::cout << "Количество терминалов в атомах не совпало\n";
      return false;
    }

    for (size_t i = 0; i < terminals_size; ++i) {
      const auto* t1 = atom1_.terminals[i];
      const auto* t2 = atom2_.terminals[i];
      std::cout << "\n[Пара " << i + 1 << "] " << t1 << " — " << t2 << '\n';

      if (!UnifyPair(t1, t2)) {
        return false;
      }

      PrintLinks();
    }
    return true;
  }

 private:
  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)
  const Atom& atom1_;
  const Atom& atom2_;
  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)

  // Таблица подстановок: VariableName -> Constant.
  std::unordered_map<NameSV, const Constant*> variables;

  // Список связей (множество согласований): ConstantName -> множество
  // VariableName.
  std::unordered_map<NameSV, std::set<NameSV>> links;

  // Нераспространённые связи
  using UnresolvedLink = std::set<NameSV>;
  using UnresolvedLinks = std::list<UnresolvedLink>;
  using UnresolvedLinkIt = UnresolvedLinks::iterator;
  UnresolvedLinks unresolved_links;

  [[nodiscard]] UnresolvedLinkIt FindUnresolvedLinkFor(NameSV variable_name) {
    return std::find_if(unresolved_links.begin(), unresolved_links.end(),
                        [variable_name](const auto& group) {
                          return group.contains(variable_name);
                        });
  }

  void ResolveLink(UnresolvedLinkIt linkIt, const Constant& constant) {
    for (auto item : *linkIt) {
      variables[item] = &constant;
      links[constant.value()].emplace(item);
      std::cout << "Согласуем подстановку " << item << " = " << constant
                << '\n';
    }
    unresolved_links.erase(linkIt);
  }

  [[nodiscard]] bool UnifyPair(TerminalCPtr t1, TerminalCPtr t2) {
    if (t1->IsVariable()) {
      const auto& v1 = t1->AsVariable();
      if (t2->IsVariable()) {
        return UnifyVars(v1, t2->AsVariable());
      }
      return UnifyVarAndConst(v1, t2->AsConstant());
    }

    const auto& c1 = t1->AsConstant();
    if (t2->IsVariable()) {
      return UnifyVarAndConst(t2->AsVariable(), c1);
    }
    return UnifyConsts(c1, t2->AsConstant());
  }

  [[nodiscard]] bool UnifyVars(const Variable& var1, const Variable& var2) {
    auto it1 = variables.find(var1.name);
    auto it2 = variables.find(var2.name);

    if (it1 == variables.end()) {
      std::cout << "Переменная " << var1 << " встречается первый раз\n";
      if (it2 == variables.end()) {
        std::cout << "Переменная " << var2 << " встречается первый раз\n"
                  << "Свяжем эти переменные\n";
        variables.emplace(var1.name, nullptr);
        variables.emplace(var2.name, nullptr);
        unresolved_links.push_back(UnresolvedLink{var1.name, var2.name});
        return true;
      }
      if (it2->second == nullptr) {
        std::cout
            << "Переменная " << var2
            << " уже встречалась ранее, но на неё не распространена константа\n"
            << "Добавим " << var1 << " к связи " << var2 << '\n';
        variables.emplace(var1.name, nullptr);
        FindUnresolvedLinkFor(var2.name)->emplace(var1.name);
        return true;
      }
      std::cout << "Распространим подстановку " << var2 << " = " << it2->second
                << " и на " << var1 << '\n';
      variables.emplace(var1.name, it2->second);
      links[it2->second->name].emplace(var1.name);
      return true;
    }

    if (it1->second == nullptr) {
      std::cout
          << "Переменная " << var1
          << " уже встречалась ранее, но на неё не распространена константа\n";
      auto link1 = FindUnresolvedLinkFor(var1.name);
      if (it2 == variables.end()) {
        std::cout << "Переменная " << var2
                  << " встречается первый раз, добавим её к связи " << var1
                  << '\n';
        variables.emplace(var2.name, nullptr);
        link1->emplace(var2.name);
        return true;
      }
      if (it2->second == nullptr) {
        std::cout << "Переменная " << var2
                  << " уже встречалась ранее, но на неё не распространена "
                     "константа\n";
        auto link2 = FindUnresolvedLinkFor(var2.name);
        if (link1 != link2) {
          std::cout << "Переменные находятся в разных связях, объединим их\n";
          link1->insert(link2->begin(), link2->end());
          unresolved_links.erase(link2);
          return true;
        }
        std::cout
            << "Переменные уже находятся в одной связи, ничего не делаем\n";
        return true;
      }
      std::cout << "У переменной " << var2 << " согласована подстановка "
                << it2->second << ", распространим её и на связь " << var1
                << '\n';
      ResolveLink(link1, *it2->second);
      return true;
    }

    std::cout << "Подстановка " << var1 << " = " << it1->second << '\n';

    if (it2 == variables.end()) {
      std::cout << "Переменная " << var2 << " встречается первый раз\n";
      variables.emplace(var2.name, it1->second);
      links[it1->second->name].emplace(var2.name);
      return true;
    }

    if (it2->second == nullptr) {
      std::cout
          << "Переменная " << var2
          << " уже встречалась ранее, но на неё не распространена константа\n";
      auto link2 = FindUnresolvedLinkFor(var2.name);
      ResolveLink(link2, *it1->second);
      return true;
    }

    std::cout << "Подстановка " << var2 << " = " << it2->second << '\n';

    const auto is_equal = (it1->second->value() == it2->second->value());
    std::cout << "Подстановки " << (is_equal ? "" : "не") << " совпали\n";

    return is_equal;
  }

  [[nodiscard]] bool UnifyVarAndConst(const Variable& variable,
                                      const Constant& constant) {
    auto it = variables.find(variable.name);
    if (it == variables.end()) {
      variables.emplace(variable.name, &constant);
      links[constant.value()].emplace(variable.name);
      std::cout << "Согласуем подстановку " << variable << " = " << constant
                << '\n';
    } else if (it->second == nullptr) {
      auto linkIt = FindUnresolvedLinkFor(variable.name);
      ResolveLink(linkIt, constant);
    } else if (it->second->value() == constant.value()) {
      std::cout << "Подстановка " << variable << " совпала с " << constant
                << '\n';
    } else {
      std::cout << "Подстановка " << variable << " = " << GetValue(variable)
                << " не совпала с " << constant << '\n';
      return false;
    }

    return true;
  }

  [[nodiscard]] static bool UnifyConsts(const Constant& const1,
                                        const Constant& const2) {
    if (const1.value() != const2.value()) {
      std::cout << "Константы не равны: " << const1.value() << " != " << const2.value() << '\n';
      return false;
    }
    std::cout << "Константы равны: " << const1.value() << " == " << const2.value() << '\n';
    return true;
  }

  [[nodiscard]] std::string_view GetValue(const Variable& variable) {
    auto it = variables.find(variable.name);
    if (it == variables.end() || it->second == nullptr) {
      return "NULL";
    }
    return it->second->value();
  }

  void PrintLinks() {
    for (const auto& [constant_name, variable_names] : links) {
      std::cout << '{';
      join(variable_names, std::cout);
      std::cout << "}: " << constant_name << '\n';
    }
    for (const auto& group : unresolved_links) {
      std::cout << '{';
      join(group, std::cout);
      std::cout << "}: ???\n";
    }
  }
};

}  // namespace

bool Unify(const Atom& atom1, const Atom& atom2) {
  return Unification{atom1, atom2}.Run();
}
