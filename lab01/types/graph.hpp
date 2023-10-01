#pragma once

#include <cstddef>

#include <types/common.hpp>


using Vertex = std::size_t;
using Vertices = Set<Vertex>;

using Edge = Pair<Vertex, Vertex>;
using Edges = Vector<Edge>;
