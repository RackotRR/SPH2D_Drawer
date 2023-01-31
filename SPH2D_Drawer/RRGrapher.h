#pragma once
#include <iostream>
#include <vector>

#include "Particle.h"

using Square = std::pair<std::pair<double, double>, std::pair<double, double>>;
using TimeLayer = std::vector<Particle>; 
using Grid = std::vector<TimeLayer>;

class RRGrapher {
public:
	static RRGrapher& Instance();

	void Show(Grid grid, Square area, double particleSize);
private:
	void DrawLayer() const;

	void ComputeStartScale();
	void Stop();

	void RunWindowCycle();
	void UpdateControls();

	RRGrapher() = default;
	RRGrapher(RRGrapher&) = delete;
	void operator=(RRGrapher&) = delete;

	bool shallStop{ false };

	bool autoPlay{ false };
	unsigned timeToLayer{ 1 };
	unsigned passedTime{};
	  
	Grid grid;
	Square area;
	
	double particleSize{};
	double scaleCoord{ 1 }; 
	double deltaX{};
	double deltaY{};

	size_t currentLayer{};
};