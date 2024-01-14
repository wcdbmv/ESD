#pragma once

#include <vector>
#include <initializer_list>

#include "clause.h"

class Formula {
public:
  Formula(std::initializer_list<Clause> clauses);

  [[nodiscard]] const std::vector<Clause>& clauses() const;
  [[nodiscard]] std::vector<Clause>& clauses();

private:
  std::vector<Clause> clauses_;
};
