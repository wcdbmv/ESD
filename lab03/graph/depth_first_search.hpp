#pragma once

#include <types/graph.hpp>

class HyperGraph;

struct SearchResult {
  Vector<Vertex> closed_vertices;
  Vector<size_t> closed_rules;
  bool found = false;
};

SearchResult depthFirstSearch(const HyperGraph& graph,
                              Vertex from,
                              const Vertices& to);
