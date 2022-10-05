#include <iostream>

#include "RRGrapher.h"  
#include "FileInput.h"


int main(int argc, char* argv[]) {      
	std::string str;
	std::cout << "experiment directory: " << std::endl;
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