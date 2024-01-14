#pragma once

#include "types/name.h"
#include "types/terminal.h"

#include <iosfwd>
#include <vector>

struct Atom {
  Name name;

  using Terminals = std::vector<TerminalCPtr>;
  Terminals terminals;
};

std::ostream& operator<<(std::ostream& os, const Atom& atom);
