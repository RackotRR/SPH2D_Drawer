#include <iostream>
#include <string>

#include "RRGrapher.h"  

int main(int argc, char* argv[]) {      
	std::string experiment_name;
	std::cout << "[SPH2D Drawer] experiment name: " << std::endl;
	std::getline(std::cin, experiment_name);

	try {
		RRGrapher::Instance().SetupHeatMap(0, 0, "r");
		RRGrapher::Instance().Init(experiment_name);
		RRGrapher::Instance().Show();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
	}

	return 0;
}