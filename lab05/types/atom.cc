#include "types/atom.h"

#include <ostream>

#include "utils/io_join.h"

std::ostream& operator<<(std::ostream& os, const Atom& atom) {
  os << atom.name << '(';
  join(atom.terminals, os);
  return os << ')';
}
