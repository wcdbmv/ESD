#include "terminal.h"

#include <ostream>
#include <string_view>

Terminal Terminal::Constant(const std::string_view name) {
  return {name, Type::kConstant};
}

Terminal Terminal::Variable(const std::string_view name) {
  return {name, Type::kVariable};
}

std::string_view Terminal::name() const {
  return name_;
}

void Terminal::set_name(const std::string_view name) {
  name_ = name;
}

bool Terminal::IsConstant() const {
  return type_ == Type::kConstant;
}

bool Terminal::IsVariable() const {
  return type_ == Type::kVariable;
}

void Terminal::MakeConstant() {
  type_ = Type::kConstant;
}

std::ostream& operator<<(std::ostream& os, const Terminal& terminal) {
  return os << terminal.name_;
}

Terminal::Terminal(const std::string_view name, const Type type)
    : name_{name}, type_{type} {}
