#include "resolution.h"

#include <cstddef>
#include <iostream>
#include <string_view>
#include <utility>
#include <vector>

#include "atom.h"
#include "clause.h"
#include "io_join.h"
#include "resolvent.h"

namespace {

/// Добавление элементов вектора what в конец вектора to
template <typename T>
void Append(std::vector<T>& to, const std::vector<T>& what) {
  to.insert(to.end(), what.begin(), what.end());
}

/// Конкатенация векторов
template <typename T>
std::vector<T> Concatenate(const std::vector<T>& first,
                           const std::vector<T>& second) {
  std::vector<T> result;
  result.reserve(first.size() + second.size());

  Append(result, first);
  Append(result, second);

  return result;
}

void PrintInput(const std::vector<Clause>& axioms,
                const Clause& inverted_target) {
  std::cout << "Входные данные:\n";
  std::cout << "\tАксиомы:\n";
  for (size_t i = 0; i < axioms.size(); ++i) {
    std::cout << "\t\t" << i + 1 << ". " << axioms[i] << '\n';
  }
  std::cout << "\tЦель: " << Negate(inverted_target.front()) << '\n';
}

}  // namespace

std::string_view ToString(ResolutionResult result) {
  switch (result) {
    case ResolutionResult::kProvenByFoundingEmptyClause:
      return "ИСТИНА\nДоказано, найдена контрарная пара";
    case ResolutionResult::kMaxIterationsExceeded:
      return "ЛОЖЬ\nВыход по числу итераций";
    case ResolutionResult::kNoProofFound:
      return "ЛОЖЬ\nНе найдено доказательство";
    case ResolutionResult::kNoNewClausesAdded:
      return "ЛОЖЬ\nНовых дизъюнктов не добавилось";
  }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
ResolutionResult FullResolution(const std::vector<Clause>& axioms,
                                const Clause& inverted_target,
                                size_t max_iterations) {
  std::cout << "[Полный перебор] ";
  PrintInput(axioms, inverted_target);

  auto clause_stack = Concatenate(axioms, {inverted_target});

  for (size_t iteration = 0, i = 0; i < clause_stack.size() - 1;
       ++i) {  // Проходим

    std::cout << "[Итерация " << iteration << "] Множество дизъюнктов: {";
    join(clause_stack, std::cout);
    std::cout << "}\n";

    for (size_t j = 0; j < clause_stack.size(); ++j) {  // по
      if (i == j) {                                     // всем
        continue;                                       // парам
      }                                                 // дизъюнктов.

      ++iteration;
      if (iteration > max_iterations) {
        return ResolutionResult::kMaxIterationsExceeded;
      }

      const auto& clause1 = clause_stack[i];
      const auto& clause2 = clause_stack[j];

      // пытаемся найти резольвенту
      auto [resolvent, res] = CreateResolvent(clause1, clause2);

      if (res ==
          ResolventResult::kEmptyClause) {  // Если нашли пустой дизъюнкт,
        std::cout << "Найдена резольвента дизъюнктов " << clause1 << " и "
                  << clause2 << ": {}\n";
        return ResolutionResult::kProvenByFoundingEmptyClause;  // то доказали
                                                                // утверждение.
      }

      if (res == ResolventResult::kOk) {  // Если получилось,
        std::cout << "Найдена резольвента дизъюнктов " << clause1 << " и "
                  << clause2 << ": " << resolvent;
        auto ignore = false;
        for (const auto& clause : clause_stack) {
          if (clause == resolvent) {
            std::cout << " [игнор]";
            ignore = true;
            break;
          }
        }
        if (!ignore) {
          clause_stack.push_back(resolvent);  // то добавляем её в стек.
        }
        std::cout << '\n';
      }

      // Если резолв. дизъ. был 1, то мы его не добавляем.
    }
  }

  return ResolutionResult::kNoProofFound;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
ResolutionResult BasicResolution(const std::vector<Clause>& axioms,
                                 const Clause& inverted_target,
                                 size_t max_iterations) {
  std::cout << "[Опорное множество] ";
  PrintInput(axioms, inverted_target);

  std::vector<Clause> s2{inverted_target};

  for (size_t iteration = 0; /* nothing */; /* nothing */) {
    std::vector<Clause> clauses_to_append;

    std::cout << "[Итерация " << iteration << "] Множество дизъюнктов: {";
    join(s2, std::cout);
    std::cout << "}\n";

    for (const auto& clause2 : s2) {
      for (const auto& clause1 : axioms) {
        // Та же логика, что и в полном переборе, только добавляем не в общий
        // стек, а в s2.

        ++iteration;
        if (iteration > max_iterations) {
          return ResolutionResult::kMaxIterationsExceeded;
        }

        auto [resolvent, res] = CreateResolvent(clause1, clause2);

        if (res == ResolventResult::kEmptyClause) {
          std::cout << "Найдена резольвента дизъюнктов " << clause1 << " и "
                    << clause2 << ": {}\n";
          return ResolutionResult::kProvenByFoundingEmptyClause;
        }
        if (res == ResolventResult::kOk) {
          std::cout << "Найдена резольвента дизъюнктов " << clause1 << " и " << clause2 << ": " << resolvent;
          auto ignore = false;
          for (const auto& clause : Concatenate(s2, clauses_to_append)) {
            if (clause == resolvent) {
              std::cout << " [игнор]";
              ignore = true;
              break;
            }
          }
          if (!ignore) {
            clauses_to_append.push_back(std::move(resolvent));
          }
          std::cout << "\n";
        }
      }
    }

    if (clauses_to_append.empty()) {
      return ResolutionResult::kNoNewClausesAdded;
    }

    Append(s2, clauses_to_append);
  }
}
