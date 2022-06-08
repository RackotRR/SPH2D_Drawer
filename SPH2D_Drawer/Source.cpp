#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <functional>
#include <optional>
#include <cmath>

#include "RRGrapher.h"  

auto ReadGridAndParams(std::string_view filePath) {
	std::tuple<Grid, Square, double> tuple;
	auto& [grid, square, dx] = tuple;
	auto& [origin, size] = square;

	double dy;
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
			layer.emplace(x, y, type);
		}
		grid.push_back(std::move(layer));
	}

	return tuple;
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