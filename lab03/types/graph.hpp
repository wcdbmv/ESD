#pragma once

#include <cstddef>

#include <types/common.hpp>


using Vertex = std::size_t;
using Vertices = Set<Vertex>;


struct Edge {
	Vertex from;
	Vertices to;

	std::size_t id;
};

using Edges = Vector<Edge>;
