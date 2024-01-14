#pragma once

#include <types/graph.h>

class Graph;
struct SearchResult;

SearchResult breadthFirstSearch(const Graph& graph, Vertex from, Vertex to);
