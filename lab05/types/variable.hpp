#pragma once

#include "types/terminal.hpp"

struct Variable final : public Terminal {
  using Terminal::Terminal;

  [[nodiscard]] bool IsConstant() const noexcept override { return false; }
};
