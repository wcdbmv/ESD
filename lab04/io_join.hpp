#pragma once

#include <experimental/iterator>
#include <ostream>

template <typename It>
void join(It begin, It end, std::ostream& os, const char* delimiter = ", ") {
  std::copy(begin, end, std::experimental::make_ostream_joiner(os, delimiter));
}

template <typename Container>
void join(const Container& container,
          std::ostream& os,
          const char* delimiter = ", ") {
  join(std::begin(container), std::end(container), os, delimiter);
}
