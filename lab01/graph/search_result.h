#pragma once

#include "types/graph.h"

struct SearchResult {
  Vector<Vertex> path;
  Vector<Vertex> closed_vertices;
};
