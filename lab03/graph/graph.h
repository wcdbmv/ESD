#pragma once

#include <filesystem>
#include <iosfwd>

#include "types/graph.h"

class HyperGraph {
 public:
  explicit HyperGraph(Vertices vertices = {}, Edges edges = {});

  [[nodiscard]] const auto& vertices() const { return vertices_; }
  [[nodiscard]] const auto& edges() const { return edges_; }

  friend std::istream& operator>>(std::istream& is, HyperGraph& graph);
  friend std::ostream& operator<<(std::ostream& os, const HyperGraph& graph);

  [[nodiscard]] static HyperGraph readFromFile(
      const std::filesystem::path& path);
  void writeToFile(const std::filesystem::path& path) const;

 private:
  Vertices vertices_;
  Edges edges_;

  void checkVertex_(const Vertex& v) const;
};
