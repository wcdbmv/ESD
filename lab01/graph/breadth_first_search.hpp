#pragma once

#include <types/graph.hpp>


class Graph;
struct SearchResult;


SearchResult breadthFirstSearch(const Graph& graph, Vertex from, Vertex to);
