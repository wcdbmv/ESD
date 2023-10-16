#include <iostream>

#include <graph/depth_first_search.hpp>
#include <graph/graph.hpp>
#include <utils/io_utils.hpp>


int main() {
	const auto graph = HyperGraph::readFromFile("../tests/graph1.txt");
	std::cout << "========== Граф И/ИЛИ ==========\n" << graph;

	const Vertex from = 14;
	const Vertices to{18, 19, 20, 32};
	std::cout << "========== Цель ================\n" << from << '\n';
	std::cout << "========== Данные ==============\n" << (join(to, std::cout), '\n');

	std::cout << "========== Трассировка =========\n";
	const auto res = depthFirstSearch(graph, from, to);

	std::cout << "========== Результат ===========\n" << "Путь ";
	if (res.found) {
		std::cout << "НАЙДЕН\n";
		std::cout << "Закрытые вершины: " << (join(res.closed_vertices, std::cout), '\n');
		std::cout << "Закрытые правила: " << (join(res.closed_rules, std::cout), '\n');
	} else {
		std::cout << "НЕ НАЙДЕН\n";
	}
}
