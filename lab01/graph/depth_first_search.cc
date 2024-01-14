#include "graph/depth_first_search.h"

#include <cstddef>
#include <iterator>

#include "graph/graph.h"
#include "graph/pattern_search_result.h"
#include "graph/search_params.h"
#include "graph/search_result.h"
#include "types/graph.h"
#include "utils/convert.h"

namespace {

PatternSearchResult sPatternSearch(SearchParams& p) {
  for (size_t i = 0; i < p.edges.size(); ++i) {
    if (p.visited[i]) {
      continue;
    }

    const auto& [from, to] = p.edges[i];

    if (p.opened_vertices.front() == from) {
      p.opened_vertices.push_front(to);

      if (to == p.to) {
        return PatternSearchResult::kFoundLastEdge;
      }

      p.visited[i] = true;
      return PatternSearchResult::kFoundNextEdge;
    }
  }

  return PatternSearchResult::kNotFound;
}

}  // namespace

SearchResult depthFirstSearch(const Graph& graph, Vertex from, Vertex to) {
  SearchParams p{graph, from, to};
  p.opened_vertices.push_back(from);

  if (!p.vertices.contains(from) || !p.vertices.contains(to)) {
    return {};
  }

  if (from == to) {
    return {{from}, {}};
  }

  while (true) {
    const auto result = sPatternSearch(p);
    if (result == PatternSearchResult::kFoundLastEdge) {
      return {reverseConverted(p.opened_vertices),
              converted(p.closed_vertices)};
    }
    if (result == PatternSearchResult::kNotFound) {
      if (std::next(p.opened_vertices.begin()) != p.opened_vertices.end()) {
        p.closed_vertices.push_front(p.opened_vertices.front());
        p.opened_vertices.pop_front();
      } else if (p.opened_vertices.front() == p.from) {
        return {{}, converted(p.closed_vertices)};
      }
    }
  }
}
