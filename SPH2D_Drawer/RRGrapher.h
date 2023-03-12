#pragma once
#include <iostream>
#include <unordered_map> 
#include <functional>
#include <array>
#include <vector>

#include "Particle.h"
#include "HeatMap2.h"

using Square = std::pair<std::pair<double, double>, std::pair<double, double>>;
using TimeLayer = std::vector<Particle>; 
using Grid = std::vector<TimeLayer>;

class RRGrapher {
public:
	static RRGrapher& Instance();

	void Show(Grid grid, Square area, double particleSize);
	void SetupHeatMap(double min, double max);
private:
	void DrawLayer() const;
	void DrawLegend() const;
	void UpdateDraw() const;

	void ComputeStartScale();
	void Stop();

	void RunWindowCycle();
	void UpdateControls();

	void UpdateConsoleInput();
	void InitConsoleCommands();

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

	double spaceSpeed{ 3 };
	double deltaX{};
	double deltaY{};

	size_t currentLayer{};

	std::unordered_map<std::string, std::function<void()>> commands;
};