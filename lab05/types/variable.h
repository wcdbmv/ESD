#pragma once

#include "types/terminal.h"

struct Variable final : public Terminal {
  using Terminal::Terminal;

  [[nodiscard]] bool IsConstant() const noexcept override { return false; }
};
