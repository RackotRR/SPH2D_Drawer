#include <iostream>
#include <string>

#include "RRGrapher.h"  
#include "FileInput.h"

int main(int argc, char* argv[]) {      
	std::string str;
	std::cout << "params file: " << std::endl;
	std::getline(std::cin, str);

	try {
		auto [grid, square, dx] = ReadGridAndParams(str);
		RRGrapher::Instance().SetupHeatMap(0, 5000);
		RRGrapher::Instance().Show(std::move(grid), square, dx);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("pause");
	}

	return 0;
}