#include "graph/breadth_first_search.h"

#include <algorithm>
#include <cstddef>

#include "graph/graph.h"
#include "types/common.h"
#include "types/graph.h"

SearchResult breadthFirstSearch(const HyperGraph& graph,
                                const Vertices& from,
                                Vertex to) {
  const auto& vertices = graph.vertices();
  const auto& edges = graph.edges();

  SearchResult r;

  if (!vertices.contains(to)) {
    return r;
  }

  r.closed_vertices = Vector<Vertex>(from.begin(), from.end());
  USet<Vertex> closed_vertices_fast_lookup(from.begin(), from.end());

  if (from.contains(to)) {
    r.found = true;
    return r;
  }

  USet<size_t> closed_rules_fast_lookup;

  const auto all_closed = [&closed_vertices_fast_lookup](const Vertices& vs) {
    return std::all_of(vs.begin(), vs.end(),
                       [&closed_vertices_fast_lookup](Vertex v) {
                         return closed_vertices_fast_lookup.contains(v);
                       });
  };

  bool rule_was_added = true;
  while (!r.found && rule_was_added) {
    rule_was_added = false;

    for (auto&& [from_, tos_, id] : edges) {
      if (closed_rules_fast_lookup.contains(id)) {
        continue;
      }

      if (all_closed(tos_)) {
        r.closed_vertices.push_back(from_);
        closed_vertices_fast_lookup.insert(from_);

        r.closed_rules.push_back(id);
        closed_rules_fast_lookup.insert(id);

        rule_was_added = true;

        if (from_ == to) {
          r.found = true;
          break;
        }
      }
    }
  }

  return r;
}
