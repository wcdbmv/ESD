#pragma once

#include <types/graph.hpp>

struct SearchResult {
  Vector<Vertex> path;
  Vector<Vertex> closed_vertices;
};
