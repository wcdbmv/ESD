#pragma once

#include <string>

#include <iosfwd>

struct Atom {
	std::string name;
	bool sign;
};

Atom Negate(const Atom& atom);

bool operator==(const Atom& lhs, const Atom& rhs);

std::ostream& operator<<(std::ostream& os, const Atom& atom);
