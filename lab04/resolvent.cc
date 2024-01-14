#include "resolvent.h"

#include <string>
#include <unordered_map>
#include <utility>

#include "clause.h"

namespace {

std::unordered_map<std::string, bool> MakeClauseFastLookup(
    const AbsorbedClause& clause) {
  std::unordered_map<std::string, bool> clause_fast_lookup;
  for (auto&& atom : clause) {
    clause_fast_lookup.emplace(atom.name, atom.sign);
  }
  return clause_fast_lookup;
}

/// Вспомогательная функция для CreateResolvent.
/// Ищет имя атома из первого дизъюнкта, для которого существует атом,
/// противоположный по знаку во втором дизъюнкте.
std::string FindOppositeAtomsName(const AbsorbedClause& first_clause,
                                  const AbsorbedClause& second_clause) {
  const auto second_clause_fast_lookup = MakeClauseFastLookup(second_clause);
  for (auto&& atom : first_clause) {
    if (auto it = second_clause_fast_lookup.find(atom.name);
        it != second_clause_fast_lookup.end()) {
      const auto& [name, sign] = *it;
      if (atom.sign != sign) {
        return name;
      }
    }
  }
  return {};
}

}  // namespace

std::pair<Clause, ResolventResult> CreateResolvent(const Clause& first,
                                                   const Clause& second) {
  auto first_absorbed_clause =
      first.Absorb();  // Приводим первый дизъюнкт к виду, где атомы не
                       // повторяются.
  auto second_absorbed_clause =
      second.Absorb();  // То же самое для второго дизъюнкта.

  // если кто-то 0
  if (first_absorbed_clause.empty() || second_absorbed_clause.empty()) {
    return {{}, ResolventResult::kEmptyClause};
  }

  // если обе не 0

  // Атом из первого списка для которого ищем противоположный по знаку
  auto mb_opposite_atoms_name =
      FindOppositeAtomsName(first_absorbed_clause, second_absorbed_clause);
  if (mb_opposite_atoms_name.empty()) {
    // противоположных пар не найдено
    return {{}, ResolventResult::kOppositePairNotFound};
  }

  // если найдена противоположная пара атомов
  // заполнить список остальных атомов, не включая в него найденные
  // противоположности
  Clause resolvent;

  const auto append_resolvent_without_opposite_atom =
      [&mb_opposite_atoms_name, &resolvent](AbsorbedClause& absorbed_clause) {
        for (auto&& atom : absorbed_clause) {
          if (atom.name != mb_opposite_atoms_name) {
            resolvent.push_back(std::move(atom));
      }
    }
  };

  append_resolvent_without_opposite_atom(first_absorbed_clause);
  append_resolvent_without_opposite_atom(second_absorbed_clause);

  auto absorbed_resolvent = resolvent.Absorb();
  const auto result = absorbed_resolvent.empty() ? ResolventResult::kEmptyClause : ResolventResult::kOk;

  return {Clause{std::move(absorbed_resolvent)}, result};
}
