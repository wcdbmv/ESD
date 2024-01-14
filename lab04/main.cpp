#include <iostream>
#include <vector>

#include <atom.hpp>
#include <clause.hpp>
#include <resolution.hpp>

int main() {
  Atom a = {"A", true};
  Atom b = {"B", true};
  Atom c = {"C", true};
  Atom d = {"D", true};
  Atom not_a = {"A", false};
  Atom not_b = {"B", false};
  Atom not_c = {"C", false};
  Atom not_d = {"D", false};

  Clause ax1 = {a, b};
  Clause ax2 = {not_a, c};
  Clause ax3 = {not_b, d};
  Clause ax4 = {not_c};
  Clause ax5 = {not_d};
  Clause ax6 = {not_a};

  const std::vector<Clause> axioms = {ax1, ax2, ax3, ax4, ax5};

  // тип обхода — full, basic (полный перебор, опорное мн-во)
  const auto res = FullResolution(axioms, ax6, 1000);
  std::cout << '\n' << ToString(res);
}
