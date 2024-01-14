#pragma once

#include <types/graph.h>

class Graph;
struct SearchResult;

SearchResult depthFirstSearch(const Graph& graph, Vertex from, Vertex to);
