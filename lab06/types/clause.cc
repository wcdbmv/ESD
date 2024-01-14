#include "clause.h"

#include <algorithm>
#include <experimental/iterator>
#include <initializer_list>
#include <ostream>
#include <vector>

#include "atom.h"

Clause::Clause(const std::initializer_list<Atom> atoms /* = {} */)
    : atoms_{atoms} {}

const std::vector<Atom>& Clause::atoms() const {
  return atoms_;
}

[[nodiscard]] std::vector<Atom>& Clause::atoms() {
  return atoms_;
}

std::ostream& operator<<(std::ostream& os, const Clause& clause) {
  if (clause.atoms_.empty()) {
    return os << "<empty>";
  }

  std::copy(clause.atoms_.begin(), clause.atoms_.end(),
            std::experimental::make_ostream_joiner(os, " | "));

  return os;
}
