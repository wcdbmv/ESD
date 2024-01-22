#include "atom.h"

#include <algorithm>
#include <experimental/iterator>
#include <initializer_list>
#include <ostream>
#include <string_view>
#include <vector>

#include "terminal.h"

Atom::Atom(const std::string_view name,
           const std::initializer_list<Terminal> terminals,
           const bool is_negative /* = false */)
    : name_{name}, terminals_{terminals}, is_negative_{is_negative} {}

std::string_view Atom::name() const {
  return name_;
}

const std::vector<Terminal>& Atom::terminals() const {
  return terminals_;
}

std::vector<Terminal>& Atom::terminals() {
  return terminals_;
}

bool Atom::is_negative() const {
  return is_negative_;
}

bool Atom::AreOpposite(const Atom& lhs, const Atom& rhs) {
  return lhs.name_ == rhs.name_ &&
         lhs.terminals_ == rhs.terminals_ &&
         lhs.is_negative_ != rhs.is_negative_;
}


std::ostream& operator<<(std::ostream& os, const Atom& atom) {
  if (atom.is_negative_) {
    os << "¬";
  }

  os << atom.name_ << '(';

  std::copy(atom.terminals_.begin(), atom.terminals_.end(),
            std::experimental::make_ostream_joiner(os, ", "));

  return os << ')';
}
