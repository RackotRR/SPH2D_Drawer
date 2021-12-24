#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <functional>
#include <optional>
#include <cmath>

#include "RRGrapher.h"  

auto ReadGridAndParams(std::string_view filePath) {
	Grid grid;
	Square square;
	auto& [origin, size] = square;

	double dx, dy;
	size_t ntotal;

	std::ifstream stream{ filePath.data() };
	std::cout << "open " << filePath << std::endl;

	stream >> origin.first;
	stream >> origin.second;
	stream >> size.first;
	stream >> size.second;
	stream >> dx;
	stream >> dy;

	std::cout << "read particles" << std::endl;
	int layerCounter{};

	while (stream >> ntotal) {
		TimeLayer layer;
		std::cout << "layer " << layerCounter++ << std::endl;
		for (size_t i{}; i < ntotal; i++) {
			double x, y;
			int type;
			stream >> x >> y;
			stream >> type;
			if (type > -INFINITY) {
				layer.emplace(x, y, type);
			}
		}
		grid.push_back(layer);
	}

	return std::make_tuple(grid, square, dx);
}

int main(int argc, char* argv[]) {      
	std::string str;
	std::cout << "params file: " << std::endl;
	std::cin >> str;

	try {
		auto [grid, square, dx] = ReadGridAndParams(str);
		RRGrapher::Instance().Show(std::move(grid), square, dx);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	
	return 0;
}