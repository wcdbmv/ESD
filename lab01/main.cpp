#include <iostream>

#include <graph/breadth_first_search.hpp>
#include <graph/depth_first_search.hpp>
#include <graph/graph.hpp>
#include <graph/search_result.hpp>
#include <utils/io_utils.hpp>

/**
 * Поиск в глубину и в ширину в пространстве состояний.
 */
int main() {
	const auto graph = Graph::readFromFile("../tests/graph2.txt");
	for (auto v : graph.vertices()) {
		for (auto u : graph.vertices()) {
			if (v == u) {
				continue;
			}

			auto dfs = depthFirstSearch(graph, v, u);
			auto bfs = breadthFirstSearch(graph, v, u);
			if (!dfs.path.empty() || !bfs.path.empty()) {
				std::cout << "Найден путь от вершины " << v << " до " << u << ":\n";
				std::cout << "\tПоиском в глубину: ";
				join(dfs.path, std::cout, " -> ");
				std::cout << "\n";
				std::cout << "\tПоиском в ширину:  ";
				join(bfs.path, std::cout, " -> ");
				std::cout << std::endl;
			}
		}
	}
}
