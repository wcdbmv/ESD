#pragma once

#include <iosfwd>
#include <vector>

#include "atom.h"

class AbsorbedClause;

/// Дизъюнкт.
class Clause : public std::vector<Atom> {
 public:
  using vector::vector;

  explicit Clause(AbsorbedClause&& absorbed_clause) noexcept;

  /// Приведение дизъюнкта к виду, где атомы не повторяются, или к истине, если
  /// внутри него есть пара типа: ~A, A.
  ///
  /// @returns дизъюнкт с неповторяющимися атомами или пустой дизъюнкт в случае
  /// истины.
  [[nodiscard]] AbsorbedClause Absorb() const;
};

/// Дизъюнкт без повторов.
class AbsorbedClause : public std::vector<Atom> {
 public:
  using vector::vector;
};


std::ostream& operator<<(std::ostream& os, const Clause& clause);
std::ostream& operator<<(std::ostream& os, const AbsorbedClause& clause);
