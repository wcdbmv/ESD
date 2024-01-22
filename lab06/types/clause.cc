#include "clause.h"

#include <algorithm>
#include <experimental/iterator>
#include <initializer_list>
#include <ostream>
#include <sstream>
#include <utility>
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

void Clause::Distinct() {
  std::vector<Atom> new_atoms;
  for (const auto& atom : atoms_) {
    auto is_unique = true;
    for (const auto& new_atom : new_atoms) {
      if (atom == new_atom) {
        is_unique = false;
        break;
      }
    }
    if (is_unique) {
      new_atoms.push_back(atom);
    }
  }
  atoms_ = std::move(new_atoms);
}

void Clause::RemoveOpposites() {
  std::vector<Atom> new_atoms;
  for (const auto& i : atoms_) {
    auto found_opposite = false;
    for (const auto& j : atoms_) {
      if (Atom::AreOpposite(i, j)) {
        found_opposite = true;
        break;
      }
    }
    if (!found_opposite) {
      new_atoms.push_back(i);
    }
  }
  atoms_ = std::move(new_atoms);
}

void Clause::Sort() {
  std::sort(atoms_.begin(), atoms_.end(), [](const auto& lhs, const auto& rhs) {
    return (std::stringstream{} << lhs).str() <
           (std::stringstream{} << rhs).str();
  });
}

std::ostream& operator<<(std::ostream& os, const Clause& clause) {
  if (clause.atoms_.empty()) {
    return os << "<empty>";
  }

  std::copy(clause.atoms_.begin(), clause.atoms_.end(),
            std::experimental::make_ostream_joiner(os, " | "));

  return os;
}
