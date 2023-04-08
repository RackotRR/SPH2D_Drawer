#include <iostream>
#include <string>

#include "RRGrapher.h"  

int main(int argc, char* argv[]) {
	std::cout << "[SPH2D Drawer]" << std::endl;

	try {
		RRGrapher::Instance().SetupHeatMap(0, 0, "r");
		RRGrapher::Instance().Init();
		RRGrapher::Instance().Show();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
	}

	return 0;
}