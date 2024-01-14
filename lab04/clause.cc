#include "clause.h"

#include <unordered_map>
#include <utility>
#include <ostream>
#include <string>
#include <vector>

#include "atom.h"
#include "io_join.h"

Clause::Clause(AbsorbedClause&& absorbed_clause) noexcept
    : std::vector<Atom>(std::move(absorbed_clause)) {}

AbsorbedClause Clause::Absorb() const {
  AbsorbedClause absorbed;  // Дизъюнкт без дублей.
  std::unordered_map<std::string, bool> processed_atoms;

  for (auto&& atom : *this) {
    // Если имя атома совпало с именем проверенного атома,
    if (auto it = processed_atoms.find(atom.name);
        it != processed_atoms.end()) {
      const auto& [name, sign] = *it;
      if (atom.sign != sign) {
        // но знаки у них разные — значит он просто 1.
        return {};
      }
      // Если знаки и имена совпали — не добавляем этот атом в результат.
      continue;
    }
    // Если такого атома не было в "дизъюнкте без дублей", то добавляем его
    // туда.
    absorbed.push_back(atom);
    processed_atoms.emplace(atom.name, atom.sign);
  }

  return absorbed;
}

std::ostream& operator<<(std::ostream& os, const Clause& clause) {
  os << "{";
  join(clause, os);
  return os << "}";
}

std::ostream& operator<<(std::ostream& os, const AbsorbedClause& clause) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return os << reinterpret_cast<const Clause&>(clause);
}
