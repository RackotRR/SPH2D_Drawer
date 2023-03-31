#include <limits>
#include <algorithm>
#include <iostream>
#include <format>

#include "RRGrapher.h"
#include "RRGameIO.h"
#include "RRController.h"
#include "RRGameTime.h"  

RRGrapher& RRGrapher::Instance() {
	static RRGrapher instance;
	return instance;
}

void RRGrapher::SetupHeatMap(double min, double max, std::string variableName) {
	heatMap.SetNew(min, max, variableName);
}

void RRGrapher::Init(std::string experiment_name) {
	sphfio = std::make_unique<SPHFIO>(experiment_name);

	grid = sphfio->takeGrid();
	area = sphfio->getSquare();
	particleSize = params.delta;
	simulationTimePerLayer = params.dt * params.save_step;
}
void RRGrapher::Show() {
	auto& gameIO{ RRGameIO::Instance() };
	try { 
		gameIO.Initialize();
		InitConsoleCommands();
		DefaultSetup();

		this->passedTime = 0;

		if (grid.empty()) {
			throw std::runtime_error("Grid had no time layers!");
		} 

		ComputeStartScale();

		RunWindowCycle();

		gameIO.Shutdown();
	}
	catch (const std::exception& e) {
		gameIO.Shutdown();
		throw;
	}
}

void RRGrapher::DrawLayer() const {
	auto& layer{ grid[currentLayer] };
	auto& gameIO{ RRGameIO::Instance() };	  

#define GET_VARIABLE_BY_NAME(variable) { #variable, [](const Particle& part) { return part.variable; } }
	static std::unordered_map<std::string, std::function<double(const Particle& part)>> getValueFunctions{
		GET_VARIABLE_BY_NAME(vx),
		GET_VARIABLE_BY_NAME(vy),
		GET_VARIABLE_BY_NAME(p),
		GET_VARIABLE_BY_NAME(rho),
		GET_VARIABLE_BY_NAME(u)
	};

	// ���������� ������� ��������:
	const auto& [origin, size] = area;
	double x0{ origin.first };
	double x1{ origin.first + size.first };
	double y0{ origin.second };
	double y1{ origin.second + size.second };

	double bottomY{ gameIO.GetWinHeight() * 0.8 };

	auto toScreenX{ 
		[=](double x) {
			return static_cast<int>(deltaX + x * scaleCoord);
		}
	};
	auto toScreenY{
		[=](double y) {
			return static_cast<int>(bottomY - (deltaY + y * scaleCoord));
		}
	};

	constexpr RRColor realColor = RRColor::Blue();
	constexpr RRColor virtualColor = RRColor::Black();
	 
	for (auto& particle : layer) {
		Vector2 pos{ toScreenX(particle.x), toScreenY(particle.y) };

		auto& variableName = heatMap.GetVariableName();
		if (getValueFunctions.contains(variableName)) {
			auto getValueFunction = getValueFunctions[variableName];
			auto value = getValueFunction(particle);
			gameIO.DrawRectangle(Rectangle{ pos.X, pos.Y,
				1 + int(particleSize * scaleCoord),
				1 + int(particleSize * scaleCoord) },
				heatMap.GetNewColorForNum(value));
		}
		else {
			gameIO.DrawRectangle(Rectangle{ pos.X, pos.Y, 
				1 + int(particleSize * scaleCoord), 
				1 + int(particleSize * scaleCoord) },
				particle.itype == 2 ? realColor : virtualColor);
		}
	}
}


void RRGrapher::DrawLegend() const {
	auto legend = heatMap.GetLegend();
	if (legend.size() == 1) return;

	auto& gameIO{ RRGameIO::Instance() };
	auto w = gameIO.GetWinWidth();
	auto h = gameIO.GetWinHeight();

	int posX = w * 0.8;
	int posY = h * 0.05;
	int rectX = w * 0.7;
	int rectY = h * 0.04;
	int rectWidth = w * 0.08;
	int rectHeight = h * 0.05;

	int i = 0;
	for (auto& [value, color] : legend) {
		auto dy = posY * i++;
		auto line = std::to_string(value);
		int symbols = value < 0 ? 5 : 4;
		gameIO.DrawLine({ posX, posY + dy }, Font::Menu, std::string(std::begin(line), std::begin(line) + symbols));
		gameIO.DrawRectangle(Rectangle{ rectX, rectY + dy, rectWidth, rectHeight }, color);
	}
	gameIO.DrawLine({ int(w * 0.7), 0 }, Font::Menu, heatMap.GetVariableName());
}

void RRGrapher::DrawTime() const {
	constexpr double t0 = 0;
	double t = t0 + currentLayer * simulationTimePerLayer;
	auto& gameIO{ RRGameIO::Instance() };
	int w = gameIO.GetWinWidth();
	RRGameIO::Instance().DrawLine({ (int)(w * 0.05f), 0}, Font::Menu, std::format("{:.3f}", t));
}


void RRGrapher::ComputeStartScale() { 
	auto& gameIO{ RRGameIO::Instance() };
	int startX{};
	int startY{};
	int endX{ gameIO.GetWinWidth() };
	int endY{ gameIO.GetWinHeight() };

	double areaWidth{ area.second.first };
	double areaHeight{ area.second.second };
	double areaX{ area.first.first };
	double areaY{ area.first.second };

	if (gameIO.GetWinWidth() <= gameIO.GetWinHeight()) {
		scaleCoord = endY / areaHeight;		
	}
	else {
		scaleCoord = endX / areaWidth;
	}

	deltaX = startX - areaX * scaleCoord;
	deltaY = startY - areaY * scaleCoord;
}

void RRGrapher::Stop() {
	shallStop = true;
}

void RRGrapher::DefaultSetup() {
	currentLayer = 0;
	deltaX = 0;
	deltaY = 0;
	spaceSpeed = DEFAULT_SPACE_SPEED;
	scaleCoord = DEFAULT_SCALE_COORD;
	lastRenderedLayer = ULLONG_MAX;
	renderFrameCounter = 0;
	videoCounter = 0;
	timeToLayer = DEFAULT_TIME_TO_LAYER;
	autoPlay = false;
}

void RRGrapher::RunWindowCycle() { 
	auto& controller{ RRController::Instance() };
	auto& gameTime{ RRGameTime::Instance() };
	auto& gameIO{ RRGameIO::Instance() };
	 
	do {
		controller.Update();
		gameTime.Update();

		if (controller.HasQuitEvent()) {
			Stop();
		}
		 
		UpdateControls();
		UpdateAutoPlay();
		UpdateDraw();
	} while (!shallStop);
}

void RRGrapher::UpdateAutoPlay() {
	if (autoPlay) {
		passedTime += RRGameTime::Instance().GetPassedTime();
		if (passedTime > timeToLayer) {
			passedTime = 0;
			if (currentLayer < grid.size() - 1) {
				currentLayer++;
			}
			else {
				autoPlay = false;
			}
		}
	}
}

void RRGrapher::UpdateDraw() const {
	auto& gameIO{ RRGameIO::Instance() };
	gameIO.Begin(RRColor::White());
	DrawLayer();
	DrawLegend();
	DrawTime();
	gameIO.End();
}