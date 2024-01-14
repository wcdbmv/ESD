#pragma once

#include <string_view>
#include <utility>

#include "clause.h"

enum class ResolventResult {
  kOk,                    ///< Резольвента сформирована
  kOppositePairNotFound,  ///< Не найдено контрарной пары
  kEmptyClause,           ///< Был пустой дизъюнкт
};

/// Создание дизъюнкта, исключая из него контрарную пару типа: (~A, B) , (C, A)
/// = (B, C) или получение пустого дизъюнкта.
std::pair<Clause, ResolventResult> CreateResolvent(const Clause& first, const Clause& second);
