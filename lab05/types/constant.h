#pragma once

#include "types/terminal.h"

struct Constant final : public Terminal {
  using Terminal::Terminal;

  [[nodiscard]] bool IsConstant() const noexcept override { return true; }
  [[nodiscard]] std::string_view value() const noexcept { return name; }
};
