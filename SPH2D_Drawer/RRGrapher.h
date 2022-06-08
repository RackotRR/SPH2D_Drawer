#pragma once
#include <iostream>
#include <list> 
#include <array>
#include <set>

#include "Particle.h"
#include "HeatMap2.h"

using Square = std::pair<std::pair<double, double>, std::pair<double, double>>;
using TimeLayer = std::multiset<Particle>; 
using Grid = std::list<TimeLayer>;

class RRGrapher {
public:
	static RRGrapher& Instance();

	void Show(Grid grid, Square area, double particleSize);
	void SetupHeatMap(double min, double max);
private:
	void DrawLayer() const;
	void DrawLegend() const;

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
	HeatMap heatMap;

	double particleSize{};
	double scaleCoord{ 1 }; 
	double deltaX{};
	double deltaY{};

	std::list<TimeLayer>::iterator currentLayer;
	size_t currentT{};
	size_t maxT{};
};