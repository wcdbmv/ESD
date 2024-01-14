#include <iostream>
#include <exception>

#include "types/atom.h"
#include "types/constant.h"
#include "types/variable.h"
#include "unification.h"

int main() {
  try {
    auto v1 = Variable{"v1"};
    auto v2 = Variable{"v2"};
    auto v3 = Variable{"v3"};
    auto v4 = Variable{"v4"};
    auto v5 = Variable{"v5"};
    auto v6 = Variable{"v6"};
    auto v7 = Variable{"v7"};
    auto v8 = Variable{"v8"};
    auto v9 = Variable{"v9"};
    auto v10 = Variable{"v10"};
    auto v11 = Variable{"v11"};
    auto c1 = Constant{"1"};
    auto c2 = Constant{"2"};
    auto a1 = Atom{
      "A", {&v1, &v3, &v4, &v5, &v1, &v1, &v7, &v9, &v7, &v10, &v8, &v11, &v4}};
    auto a2 = Atom{
      "A", {&v2, &v1, &c1, &v4, &v6, &v5, &v8, &v10, &v9, &v7, &v1, &c2, &v11}};
    const auto res = Unify(a1, a2);
    std::cout << '\n' << (res ? "SUCCESS" : "FAILURE") << '\n';
  } catch (const std::exception& exception) {
    std::cerr << "[exception] " << exception.what() << '\n';
    return 1;
  }
}
