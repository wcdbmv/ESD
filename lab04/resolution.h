#pragma once

#include <utility>
#include <vector>

#include "clause.h"

enum class ResolutionResult {
  kProvenByFoundingEmptyClause,
  kMaxIterationsExceeded,
  kNoProofFound,
  kNoNewClausesAdded,
};
std::string_view ToString(ResolutionResult result);

/// Полный перебор.
ResolutionResult FullResolution(const std::vector<Clause>& axioms,
                                const Clause& inverted_target,
                                size_t max_iterations);

/// Опорное множество.
ResolutionResult BasicResolution(const std::vector<Clause>& axioms,
                                 const Clause& inverted_target, size_t max_iterations);
