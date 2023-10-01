#pragma once

#include <types/graph.hpp>


class Graph;


struct SearchParams {
	SearchParams(const Graph& graph, Vertex from, Vertex to);

	const Vertices& vertices;
	const Edges& edges;

	Vertex from;
	Vertex to;

	List<Vertex> opened_vertices;
	List<Vertex> closed_vertices;

	Vector<bool> visited;
};
