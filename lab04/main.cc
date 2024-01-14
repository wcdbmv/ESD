#include <exception>
#include <iostream>
#include <vector>

#include "atom.h"
#include "clause.h"
#include "resolution.h"

int main() {
  try {
    const Atom a = {"A", true};
    const Atom b = {"B", true};
    const Atom c = {"C", true};
    const Atom d = {"D", true};
    const Atom not_a = {"A", false};
    const Atom not_b = {"B", false};
    const Atom not_c = {"C", false};
    const Atom not_d = {"D", false};

    const Clause ax1 = {a, b};
    const Clause ax2 = {not_a, c};
    const Clause ax3 = {not_b, d};
    const Clause ax4 = {not_c};
    const Clause ax5 = {not_d};
    const Clause ax6 = {not_a};

    const std::vector<Clause> axioms = {ax1, ax2, ax3, ax4, ax5};

    // тип обхода — full, basic (полный перебор, опорное мн-во)
    const auto res = FullResolution(axioms, ax6, 1000);
    std::cout << '\n' << ToString(res);
  } catch (const std::exception& exception) {
    std::cerr << "[exception] " << exception.what() << '\n';
    return 1;
  }
}
