#pragma once

#include <types/graph.hpp>


class Graph;
class SearchResult;


SearchResult depthFirstSearch(const Graph& graph, Vertex from, Vertex to);
