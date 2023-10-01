#pragma once

#include <filesystem>
#include <istream>
#include <ostream>

#include <types/graph.hpp>


class Graph {
public:
	explicit Graph(Vertices vertices = {}, Edges edges = {});

	[[nodiscard]] const auto& vertices() const { return vertices_; }
	[[nodiscard]] const auto& edges() const { return edges_; }

	friend std::istream& operator>>(std::istream& is, Graph& graph);
	friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

	static Graph readFromFile(const std::filesystem::path& path);
	void writeToFile(const std::filesystem::path& path) const;

private:
	Vertices vertices_;
	Edges edges_;

	void checkVertex_(const Vertex& v) const;
};
