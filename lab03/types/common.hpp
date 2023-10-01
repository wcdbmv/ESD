#pragma once

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>


template <typename T>
using Set = std::set<T>;

template <typename Key, typename T>
using UMap = std::unordered_map<Key, T>;

template <typename T>
using USet = std::unordered_set<T>;

template <typename T>
using Vector = std::vector<T>;
