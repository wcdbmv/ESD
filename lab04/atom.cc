#include "atom.h"

#include <ostream>

Atom Negate(const Atom& atom) {
  return {atom.name, !atom.sign};
}

bool operator==(const Atom& lhs, const Atom& rhs) {
  return lhs.name == rhs.name && lhs.sign == rhs.sign;
}

std::ostream& operator<<(std::ostream& os, const Atom& atom) {
  if (!atom.sign) {
    os << "¬";
  }
  return os << atom.name;
}
