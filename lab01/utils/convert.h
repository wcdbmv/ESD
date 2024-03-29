#pragma once

#include "types/common.h"

template <typename T>
Vector<T> converted(List<T> list) {
  return {list.begin(), list.end()};
}

template <typename T>
Vector<T> reverseConverted(List<T> list) {
  return {list.rbegin(), list.rend()};
}
