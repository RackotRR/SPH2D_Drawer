#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <functional>
#include <optional>
#include <cmath>

#include "RRGrapher.h"  

auto ReadGridAndParams() {
	Grid grid;
	Square square;
	auto& [origin, size] = square;

	double dx, dy;
	size_t ntotal;

	std::ifstream stream{ "params.txt" };
	stream >> origin.first;
	stream >> origin.second;
	stream >> size.first;
	stream >> size.second;
	stream >> dx;
	stream >> dy;

	while (stream >> ntotal) {
		TimeLayer layer;
		for (size_t i{}; i < ntotal; i++) {
			double x, y;
			int type;
			stream >> x >> y;
			stream >> type;
			if (type != 0) {
				layer.emplace(x, y);
			}
		}
		grid.push_back(layer);
	}

	return std::make_tuple(grid, square, dx);
}


int main(int argc, char* argv[]) {       
	auto [grid, square, dx] = ReadGridAndParams(); 
	try {
		RRGrapher::Instance().Show(std::move(grid), square, dx);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	
	return 0;
}