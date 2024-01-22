#pragma once

#include <string>
#include <string_view>
#include <iosfwd>
#include <initializer_list>
#include <vector>

#include "terminal.h"

class Atom {  // Predicate
 public:
  Atom(std::string_view name,
       std::initializer_list<Terminal> terminals,
       bool is_negative = false);

  [[nodiscard]] std::string_view name() const;
  [[nodiscard]] const std::vector<Terminal>& terminals() const;
  [[nodiscard]] std::vector<Terminal>& terminals();
  [[nodiscard]] bool is_negative() const;

  [[nodiscard]] friend bool operator==(const Atom&, const Atom&) = default;
  [[nodiscard]] static bool AreOpposite(const Atom&, const Atom&);

  friend std::ostream& operator<<(std::ostream&, const Atom&);

 private:
  std::string name_;
  std::vector<Terminal> terminals_;
  bool is_negative_;
};
