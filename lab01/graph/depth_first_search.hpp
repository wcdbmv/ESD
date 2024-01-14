#pragma once

#include <types/graph.hpp>


class Graph;
struct SearchResult;


SearchResult depthFirstSearch(const Graph& graph, Vertex from, Vertex to);
