#include "types/atom.hpp"

#include "utils/io_join.hpp"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const Atom& atom) {
  os << atom.name << '(';
  join(atom.terminals, os);
  return os << ')';
}
