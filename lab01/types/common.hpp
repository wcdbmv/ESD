#pragma once

#include <list>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>


template <typename T>
using List = std::list<T>;

template <typename T>
using Set = std::set<T>;

template <typename Key, typename T>
using UMap = std::unordered_map<Key, T>;

template <typename T, typename U>
using Pair = std::pair<T, U>;

template <typename T>
using Vector = std::vector<T>;
