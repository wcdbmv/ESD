#include "graph.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "types/graph.h"
#include "utils/io_utils.h"

Graph::Graph(Vertices vertices, Edges edges)
    : vertices_{std::move(vertices)}, edges_{std::move(edges)} {
  for (const auto& [from, to] : edges_) {
    checkVertex_(from);
    checkVertex_(to);
  }
}

void Graph::checkVertex_(const Vertex& v) const {
  if (!vertices_.contains(v)) {
    throw std::invalid_argument("[Graph::checkVertex_] \"" + std::to_string(v) +
                                "\" is not in V");
  }
}

std::istream& operator>>(std::istream& is, Graph& graph) {
  size_t n = 0;
  is >> n;

  Vertices vertices;

  for (size_t i = 0; i < n; ++i) {
    Vertex v{};
    is >> v;
    vertices.insert(v);
  }

  is >> n;

  Edges edges;

  for (size_t i = 0; i < n; ++i) {
    Vertex from{};
    is >> from;

    std::string symbol;  // "->";
    is >> symbol;

    Vertex to{};
    is >> to;

    edges.emplace_back(from, to);
  }

  graph = Graph(std::move(vertices), std::move(edges));

  return is;
}

std::ostream& operator<<(std::ostream& os, const Graph& graph) {
  os << graph.vertices_.size() << "\n";
  join(graph.vertices_, os);
  os << "\n";

  os << graph.edges_.size() << "\n";
  for (auto&& [from, to] : graph.edges_) {
    os << from << " -> " << to << "\n";
  }
  return os;
}

Graph Graph::readFromFile(const std::filesystem::path& path) {
  Graph graph;
  std::ifstream stream{path};
  if (!stream) {
    throw std::invalid_argument("[Graph::readFromFile] " + path.string() +
                                ": no such file");
  }
  stream >> graph;
  return graph;
}

void Graph::writeToFile(const std::filesystem::path& path) const {
  std::ofstream stream{path};
  if (!stream) {
    throw std::invalid_argument("[Graph::writeToFile] " + path.string() +
                                ": can't open file");
  }
  stream << *this;
}
