#include "graph.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "types/common.h"
#include "types/graph.h"
#include "utils/io_utils.h"

HyperGraph::HyperGraph(Vertices vertices, Edges edges)
    : vertices_{std::move(vertices)}, edges_{std::move(edges)} {
  USet<size_t> ids;

  for (const auto& [from, tos, id] : edges_) {
    checkVertex_(from);
    for (auto&& to : tos) {
      checkVertex_(to);
    }
    if (ids.contains(id)) {
      throw std::invalid_argument(
          "[HyperGraph::HyperGraph] Production rule with id \"" +
          std::to_string(id) + "\" already exists");
    }
    ids.insert(id);
  }
}

void HyperGraph::checkVertex_(const Vertex& v) const {
  if (!vertices_.contains(v)) {
    throw std::invalid_argument("[HyperGraph::checkVertex_] \"" +
                                std::to_string(v) + "\" is not in V");
  }
}

std::istream& operator>>(std::istream& is, HyperGraph& graph) {
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
    std::size_t id = 0;
    is >> id;

    std::string symbol;
    is >> symbol;  // ","

    Vertex from{};
    is >> from;

    is >> symbol;  // "->"

    std::string line;
    std::getline(is, line);
    std::stringstream ss{line};

    Vertex to{};
    Vertices tos;
    while (ss >> to) {
      tos.insert(to);
    }

    edges.push_back(Edge{from, std::move(tos), id});
  }

  graph = HyperGraph(std::move(vertices), std::move(edges));

  return is;
}

std::ostream& operator<<(std::ostream& os, const HyperGraph& graph) {
  os << graph.vertices_.size() << '\n';
  join(graph.vertices_, os);
  os << '\n';

  os << graph.edges_.size() << '\n';
  for (auto&& [from, to, id] : graph.edges_) {
    os << id << ", " << from << " -> ";
    join(to, os);
    os << '\n';
  }
  return os;
}

HyperGraph HyperGraph::readFromFile(const std::filesystem::path& path) {
  HyperGraph graph;
  std::ifstream stream{path};
  if (!stream) {
    throw std::invalid_argument("[HyperGraph::readFromFile] " + path.string() +
                                ": no such file");
  }
  stream >> graph;
  return graph;
}

void HyperGraph::writeToFile(const std::filesystem::path& path) const {
  std::ofstream stream{path};
  if (!stream) {
    throw std::invalid_argument("[HyperGraph::writeToFile] " + path.string() +
                                ": can't open file");
  }
  stream << *this;
}
