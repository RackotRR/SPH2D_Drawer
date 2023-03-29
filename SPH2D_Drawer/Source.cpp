#include <iostream>
#include <string>

#include "RRGrapher.h"  
//#include "FileInput.h"

int main(int argc, char* argv[]) {      
	std::string experiment_name;
	std::cout << "[SPH2D Drawer] experiment name: " << std::endl;
	std::getline(std::cin, experiment_name);

	try {
		auto& sphfio = SPHFIO::instance();
		sphfio.init(experiment_name);

		auto square = sphfio.getSquare();
		auto grid = sphfio.takeGrid();
		auto dx = sphfio.getParams().dx;
		auto timePerLayer = sphfio.getParams().dt * sphfio.getParams().save_step;
		RRGrapher::Instance().SetupHeatMap(0, 0, "r");
		RRGrapher::Instance().Show(std::move(grid), square, dx, timePerLayer);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
	}

	return 0;
}