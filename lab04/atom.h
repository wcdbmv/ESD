#pragma once

#include <iosfwd>
#include <string>

struct Atom {
  std::string name;
  bool sign;
};

Atom Negate(const Atom& atom);

bool operator==(const Atom& lhs, const Atom& rhs);

std::ostream& operator<<(std::ostream& os, const Atom& atom);
