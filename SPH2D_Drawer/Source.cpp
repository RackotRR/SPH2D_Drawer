#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <functional>
#include <optional>
#include <cmath>

#include "RRGrapher.h"  

auto ReadGridAndParams(std::string_view filePath, int layersN) {
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
		if (layersN != 0 &&
			layerCounter >= layersN) 
			break;

		TimeLayer layer;
		std::cout << "layer " << layerCounter++ << std::endl;
		for (size_t i{}; i < ntotal; i++) {
			double x, y;
			int type;
			double vx, vy;
			double p, rho;
			stream >> x >> y;
			stream >> type;
			stream >> vx >> vy;
			stream >> p >> rho;
			layer.emplace(x, y, type, vx, vy, p, rho);
		}
		grid.push_back(std::move(layer));
	}

	return tuple;
}

int main(int argc, char* argv[]) {      
	std::string str;
	std::cout << "params file: " << std::endl;
	std::getline(std::cin, str);

	int layers;
	std::cout << "layers to read: ";
	std::cin >> layers;

	try {
		auto [grid, square, dx] = ReadGridAndParams(str, layers);
		RRGrapher::Instance().SetupHeatMap(-0.5, 0.5);
		RRGrapher::Instance().Show(std::move(grid), square, dx);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
	}

	return 0;
}