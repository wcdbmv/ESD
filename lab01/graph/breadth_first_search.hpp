#pragma once

#include <types/graph.hpp>


class Graph;
class SearchResult;


SearchResult breadthFirstSearch(const Graph& graph, Vertex from, Vertex to);
