#pragma once

#include <iosfwd>

#include "types/name.h"

struct Constant;
struct Variable;

struct Terminal {
  Name name;

  explicit Terminal(std::string_view name) : name(name.begin(), name.end()) {}

  virtual ~Terminal() = default;

  [[nodiscard]] virtual bool IsConstant() const noexcept = 0;
  [[nodiscard]] bool IsVariable() const noexcept { return !IsConstant(); }

  [[nodiscard]] const Constant& AsConstant() const noexcept;
  [[nodiscard]] const Variable& AsVariable() const noexcept;
};

using TerminalPtr = Terminal*;
using TerminalCPtr = const Terminal*;
using TerminalCRef = const Terminal&;

std::ostream& operator<<(std::ostream& os, TerminalCRef terminal);
std::ostream& operator<<(std::ostream& os, TerminalCPtr terminal);
