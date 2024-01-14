#include "types/atom.h"

#include "utils/io_join.h"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const Atom& atom) {
  os << atom.name << '(';
  join(atom.terminals, os);
  return os << ')';
}
