#include <iostream>

#include <graph/breadth_first_search.hpp>
#include <graph/graph.hpp>
#include <utils/io_utils.hpp>


int main() {
	const auto graph = HyperGraph::readFromFile("../tests/graph1.txt");

	const Vertices from{5, 6, 10, 12, 13, 19, 20};
	const Vertex to = 14;
	const auto res = breadthFirstSearch(graph, from, to);

	std::cout << "========== Граф И/ИЛИ ==========\n" << graph;
	std::cout << "========== Данные ==============\n" << (join(from, std::cout), '\n');
	std::cout << "========== Цель ================\n" << to << '\n';

	std::cout << "========== Результат ===========\n" << "Путь ";
	if (res.found) {
		std::cout << "НАЙДЕН\n";
		std::cout << "Закрытые вершины: " << (join(res.closed_vertices, std::cout), '\n');
		std::cout << "Закрытые правила: " << (join(res.closed_rules, std::cout), '\n');
	} else {
		std::cout << "НЕ НАЙДЕН\n";
	}
}
