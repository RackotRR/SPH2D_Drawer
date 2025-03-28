#pragma once
#include <iostream>
#include <unordered_map> 
#include <functional>
#include <array>
#include <vector>

#include <RRSPH_FIO.h>
#include "HeatMap.h"
#include "Vector2.h"

class RRGrapher {
public:
	static RRGrapher& Instance();

	void Init();
	void Show();
	void SetupHeatMap(double min, double max, std::string variableName);
private:
	void DrawLayer() const;
	void DrawLegend() const;
	void DrawTime() const;
	void DrawCoords() const;
	void UpdateDraw() const;

	Vector2 getScreenPos(double x, double y) const;

	void ComputeStartScale();
	void Stop();

	void RunWindowCycle();
	void UpdateControls();
	void UpdateAutoPlay();

	void UpdateConsoleInput();
	void InitConsoleCommands();
	void DefaultSetup();

	RRGrapher() = default;
	RRGrapher(RRGrapher&) = delete;
	void operator=(RRGrapher&) = delete;

	bool shallStop{ false };

	bool autoPlay{ false };

	static constexpr unsigned DEFAULT_TIME_TO_LAYER = 1;
	unsigned timeToLayer{ 1 };
	unsigned passedTime{};

	unsigned renderFrameCounter{};
	unsigned videoCounter{};
	size_t lastRenderedLayer{ ULLONG_MAX };
		  
	std::unique_ptr<sphfio::Grid> grid;
	sphfio::Square area;
	HeatMap heatMap;

	double particleSize{};

	static constexpr double DEFAULT_SCALE_COORD = 1;
	double scaleCoord{ DEFAULT_SCALE_COORD };

	static constexpr double DEFAULT_SPACE_SPEED = 3;
	double spaceSpeed{ DEFAULT_SPACE_SPEED };
	double deltaX{};
	double deltaY{};

	bool scientificMode{ true };
	bool enableCoords{ false };

	bool certainTypes{ false };
	int showType{};

	size_t currentLayer{};

	std::unordered_map<std::string, std::function<void()>> commands;
	std::unique_ptr<sphfio::SPHFIO> sphfio;
	sphfio::ParamsPtr params;
};