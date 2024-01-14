#include <graph/depth_first_search.hpp>

#include <functional>
#include <iostream>  // ugly, but teacher's requirement

#include <graph/graph.hpp>
#include <utils/io_utils.hpp>

namespace {

class Tracing {
 public:
  explicit Tracing(size_t& indent) : indent_{indent} { ++indent; }

  ~Tracing() noexcept { --indent_; }

  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << std::string(indent_, '\t') << value;
  }

 private:
  size_t& indent_;
};

}  // namespace

SearchResult depthFirstSearch(const HyperGraph& graph,
                              Vertex from,
                              const Vertices& to) {
  const auto& vertices = graph.vertices();
  const auto& edges = graph.edges();

  UMap<size_t, const Edge*> edges_lookup;
  for (auto&& edge : edges) {
    edges_lookup.emplace(edge.id, &edge);
  }

  SearchResult r;
  size_t indent = 0;

  if (!vertices.contains(from)) {
    Tracing{indent} << "Цель " << from << " не является вершиной графа\n";
    return r;
  }

  Tracing{indent} << "Кладём в список закрытых вершин данные: ";
  join(to, std::cout, ", ");
  std::cout << '\n';

  r.closed_vertices = Vector<Vertex>(to.begin(), to.end());  // proofed vertices
  USet<Vertex> closed_vertices_fast_lookup(to.begin(), to.end());

  if (to.contains(from)) {
    Tracing{indent} << "Цель " << from << " содержится в данных\n";
    r.found = true;
    return r;
  }

  USet<size_t> closed_rules_fast_lookup;

  USet<Vertex> wrong_vertices;
  USet<size_t> wrong_rules;

  std::function<bool(Vertex)> prove_node;

  const auto prove_rule = [&](size_t rule_id) {
    Tracing tracing{indent};
    tracing << "Доказываем правило " << rule_id << '\n';

    if (wrong_rules.contains(rule_id)) {
      tracing << "Ранее было выяснено, что правило " << rule_id
              << " не доказуемо\n";
      return false;
    }

    if (closed_rules_fast_lookup.contains(rule_id)) {
      tracing << "Правило " << rule_id
              << " содержится в списке закрытых правил, значит оно доказано\n";
      return true;
    }
    tracing << "Правило " << rule_id
            << " не содержится в списке закрытых правил, доказываем входящие "
               "вершины\n";
    for (auto child : edges_lookup[rule_id]->to) {
      tracing << "Проверяем вершину " << child << '\n';
      if (!prove_node(child)) {
        tracing << "Так как вершина " << child << " не доказана, то правило "
                << rule_id << " не доказано\n";
        wrong_rules.insert(rule_id);
        return false;
      }
      tracing << "Вершина " << child << " доказана\n";
    }
    tracing << "Правило " << rule_id
            << " доказано, так как все входящие вершины доказаны, помещаем его "
               "в список закрытых правил\n";
    r.closed_rules.push_back(rule_id);
    closed_rules_fast_lookup.insert(rule_id);
    return true;
  };

  prove_node = [&](Vertex v) {
    Tracing tracing{indent};
    tracing << "Доказываем вершину " << v << '\n';

    if (wrong_vertices.contains(v)) {
      tracing << "Ранее было выяснено, что вершина " << v << " не доказуема\n";
      return false;
    }

    if (closed_vertices_fast_lookup.contains(v)) {
      tracing << "Вершина " << v
              << " содержится в списке закрытых вершин, значит она доказана\n";
      return true;
    }
    tracing << "Вершина " << v
            << " не содержится в списке закрытых вершин, докажем какое-нибудь "
               "входящее правило\n";
    for (const auto& [from_, to_, id] : edges) {
      if (from_ != v) {
        continue;
      }

      tracing << "Проверяем правило " << id << '\n';
      if (prove_rule(id)) {
        tracing << "Так как правило " << id << " доказано, то вершина " << v << " доказана, помещаем её в список закрытых вершин\n";
        r.closed_vertices.push_back(v);
        closed_rules_fast_lookup.insert(v);
        return true;
      }
    }
    tracing << "Ни одно из правил, входящих в вершину " << v << " не доказано, значит вершина не доказана\n";
    wrong_vertices.insert(v);
    return false;
  };

  r.found = prove_node(from);

  return r;
}
