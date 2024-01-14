#pragma once

#include "types/graph.h"

class HyperGraph;

struct SearchResult {
  Vector<Vertex> closed_vertices;
  Vector<size_t> closed_rules;
  bool found = false;
};

SearchResult breadthFirstSearch(const HyperGraph& graph,
                                const Vertices& from,
                                Vertex to);
