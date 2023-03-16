#include <iostream>
#include <string>

#include "RRGrapher.h"  
#include "FileInput.h"

int main(int argc, char* argv[]) {      
	std::string experiment_name;
	std::cout << "[SPH2D Drawer] experiment name: " << std::endl;
	std::getline(std::cin, experiment_name);

	try {
		SPHFIO::initDrawingFilesystem(experiment_name);

		auto [grid, square, dx] = ReadGridAndParams();
		RRGrapher::Instance().SetupHeatMap(0, 0, "r");
		RRGrapher::Instance().Show(std::move(grid), square, dx);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
	}

	return 0;
}