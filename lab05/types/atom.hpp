#pragma once

#include "types/name.hpp"
#include "types/terminal.hpp"

#include <iosfwd>
#include <vector>

struct Atom {
  Name name;

  using Terminals = std::vector<TerminalCPtr>;
  Terminals terminals;
};

std::ostream& operator<<(std::ostream& os, const Atom& atom);
