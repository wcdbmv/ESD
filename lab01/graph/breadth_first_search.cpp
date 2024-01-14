#include <graph/breadth_first_search.hpp>

#include <algorithm>

#include <graph/graph.hpp>
#include <graph/pattern_search_result.hpp>
#include <graph/search_params.hpp>
#include <graph/search_result.hpp>
#include <utils/convert.hpp>

namespace {
struct BreadthFirstSearchParams : SearchParams {
  using SearchParams::SearchParams;

  UMap<Vertex, Vertex> back;
};

PatternSearchResult sPatternSearch(BreadthFirstSearchParams& p) {
  auto result = PatternSearchResult::kNotFound;

  for (size_t i = 0; i < p.edges.size(); ++i) {
    if (p.visited[i]) {
      continue;
    }

    const auto& [from, to] = p.edges[i];

    if (p.opened_vertices.front() == from) {
      p.opened_vertices.push_back(to);

      if (!p.back.contains(to)) {
        p.back[to] = from;
      }

      if (to == p.to) {
        return PatternSearchResult::kFoundLastEdge;
      }

      p.visited[i] = true;
      result = PatternSearchResult::kFoundNextEdge;
    }
  }

  return result;
}
}  // namespace

SearchResult breadthFirstSearch(const Graph& graph, Vertex from, Vertex to) {
  BreadthFirstSearchParams p{graph, from, to};
  p.opened_vertices.push_back(from);

  if (!p.vertices.contains(from) || !p.vertices.contains(to)) {
    return {};
  }

  if (from == to) {
    return {{from}, {}};
  }

  while (!p.opened_vertices.empty()) {
    const auto result = sPatternSearch(p);
    p.closed_vertices.push_front(p.opened_vertices.front());
    p.opened_vertices.pop_front();

    if (result == PatternSearchResult::kFoundLastEdge) {
      // p.closed_vertices.push_front(p.to);
      break;
    }
  }

  auto it = p.back.find(to);
  if (it == p.back.end()) {
    return {{}, converted(p.closed_vertices)};
  }

  Vector<Vertex> path{to};
  do {
    path.push_back(it->second);
    it = p.back.find(it->second);
  } while (it != p.back.end() && it->first != from);

  std::reverse(path.begin(), path.end());

  return {path, converted(p.closed_vertices)};
}
