#include <graph/search_params.hpp>

#include <graph/graph.hpp>


SearchParams::SearchParams(const Graph& graph, Vertex from, Vertex to)
	: vertices{graph.vertices()}
	, edges{graph.edges()}
	, from{from}
	, to{to}
	, visited(edges.size())
{
}
