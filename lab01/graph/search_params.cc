#include "graph/search_params.h"

#include "graph/graph.h"
#include "types/graph.h"

SearchParams::SearchParams(const Graph& graph, Vertex from, Vertex to)
    : vertices{graph.vertices()},
      edges{graph.edges()},
      from{from},
      to{to},
      visited(edges.size()) {}
