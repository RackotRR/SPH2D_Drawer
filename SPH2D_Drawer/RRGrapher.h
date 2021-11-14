#pragma once
#include <iostream>
#include <vector> 
#include <array>
#include <set>

#include "ScrollBar.h"
#include "Axis.h"

using Square = std::pair<std::pair<double, double>, std::pair<double, double>>;
using TimeLayer = std::set<std::pair<double, double>>; 
using Grid = std::vector<TimeLayer>;

class RRGrapher {
public:
	static RRGrapher& Instance();

	void Show(Grid&& grid, Square area, double particleSize);
private:
	void DrawLayer() const;

	void ComputeStartScale();
	void Stop();

	void RunWindowCycle();

	RRGrapher() = default;
	RRGrapher(RRGrapher&) = delete;
	void operator=(RRGrapher&) = delete;

	bool shallStop{ false };

	bool autoPlay{ true };
	unsigned timeToLayer{ 20 };
	unsigned passedTime{};
	  
	Grid grid;
	Square area;
	
	double particleSize;
	double scaleCoord{ 1 }; 
	double deltaX{};
	double deltaY{};

	size_t currentT{};
	size_t maxT{};  

	ScrollBar timeScroll;  
};