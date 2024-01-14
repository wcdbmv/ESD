#include "types/terminal.hpp"

#include "types/constant.hpp"
#include "types/variable.hpp"

#include <ostream>

const Constant& Terminal::AsConstant() const noexcept {
	return *dynamic_cast<const Constant*>(this);
}

const Variable& Terminal::AsVariable() const noexcept {
	return *dynamic_cast<const Variable*>(this);
}

std::ostream& operator<<(std::ostream& os, TerminalCRef terminal) {
	return os << (terminal.IsConstant() ? "const" : "var")
		  << '(' << terminal.name << ')';
}

std::ostream& operator<<(std::ostream& os, TerminalCPtr terminal) {
	return os << *terminal;
}