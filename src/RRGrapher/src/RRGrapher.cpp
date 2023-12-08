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

void RRGrapher::Init() {
	sphfio = std::make_unique<sphfio::SPHFIO>();
	params = sphfio->getParams();

	grid = std::make_unique<sphfio::Grid>(sphfio->makeGrid());
	area = sphfio::Square{ sphfio->getParams() };
	particleSize = params->delta;
}
void RRGrapher::Show() {
	auto& gameIO{ RRGameIO::Instance() };
	try { 
		gameIO.Initialize();
		InitConsoleCommands();
		DefaultSetup();

		this->passedTime = 0;

		if (grid->empty()) {
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

Vector2 RRGrapher::getScreenPos(double x, double y) const {
	double bottomY{ RRGameIO::Instance().GetWinHeight() * 0.8};

	return Vector2{
		static_cast<int>(deltaX + x * scaleCoord),
		static_cast<int>(bottomY - (deltaY + y * scaleCoord))
	};
}


void RRGrapher::DrawLayer() const {
	auto& layer{ grid->at(currentLayer) };
	auto& gameIO{ RRGameIO::Instance() };	 

	constexpr int realType = 2;
	constexpr int nonExistentType = 0;
	constexpr int virtualType = -2;
	constexpr RRColor realColor = RRColor::Blue();
	constexpr RRColor virtualColor = RRColor::Black();
		 
	auto& variableName = heatMap.GetVariableName();
	if (sphfio->isAdditionalValuePresented(variableName)) {
		for (rr_uint i = 0; i < layer.ntotal; ++i) {
			if (certainTypes && layer.itype(i) != showType) continue;
			if (layer.itype(i) == nonExistentType) continue;

			rr_float2 r = layer.r(i);
			Vector2 screenPos = getScreenPos(r.x, r.y);
			auto value = layer.getByTag(variableName, i);
			gameIO.DrawRectangle(
				Rectangle{ 
					screenPos.X, 
					screenPos.Y,
					1 + int(particleSize * scaleCoord),
					1 + int(particleSize * scaleCoord) 
				},
				layer.itype(i) == realType ? 
					heatMap.GetNewColorForNum(value) : 
					virtualColor);
		}
	}
	else {
		for (rr_iter i = 0; i < layer.ntotal; ++i) {
			if (certainTypes && layer.itype(i) != showType) continue;
			if (layer.itype(i) == nonExistentType) continue;
			
			rr_float2 r = layer.r(i);
			Vector2 screenPos = getScreenPos(r.x, r.y);
			gameIO.DrawRectangle(
				Rectangle{ 
					screenPos.X, 
					screenPos.Y,
					1 + int(particleSize * scaleCoord),
					1 + int(particleSize * scaleCoord) 
				},
				layer.itype(i) == realType ? 
					realColor : 
					virtualColor);
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
	double t = grid->at(currentLayer).time;
	auto& gameIO{ RRGameIO::Instance() };
	int w = gameIO.GetWinWidth();
	RRGameIO::Instance().DrawLine({ (int)(w * 0.05f), 0}, Font::Menu, std::format("{:.3f}", t));
}

void RRGrapher::DrawCoords() const {
	if (!enableCoords) return;
	auto& gameIO{ RRGameIO::Instance() };

	auto& mouseState = RRController::Instance().GetMouseState();
	auto& [screenX, screenY] = mouseState.Position;
	double bottomY{ gameIO.GetWinHeight() * 0.8 };
	
	double x = (screenX - deltaX) / scaleCoord;
	double y = (bottomY - screenY - deltaY) / scaleCoord;

	int w = gameIO.GetWinWidth();
	int h = gameIO.GetWinHeight();
	RRGameIO::Instance().DrawLine(
		{ (int)(w * 0.05f), (int)(h * 0.05f)},
		Font::Menu, 
		std::format("XY: ({:.3f}; {:.3f})", x, y));
}

void RRGrapher::ComputeStartScale() { 
	auto& gameIO{ RRGameIO::Instance() };
	int startX{};
	int startY{};
	int endX{ gameIO.GetWinWidth() };
	int endY{ gameIO.GetWinHeight() };

	if (gameIO.GetWinWidth() <= gameIO.GetWinHeight()) {
		scaleCoord = endY / area.size_y;		
	}
	else {
		scaleCoord = endX / area.size_x;
	}

	deltaX = startX - area.origin_x * scaleCoord;
	deltaY = startY - area.origin_y * scaleCoord;
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
	scientificMode = true;
	enableCoords = false;
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
			if (currentLayer < grid->size() - 1) {
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
	static constexpr RRColor whiteColor = RRColor::White();
	static constexpr RRColor blackColor = RRColor::Black();
	gameIO.Begin(scientificMode ? whiteColor : blackColor);

	DrawLayer();
	if (scientificMode) {
		DrawLegend();
		DrawTime();
		DrawCoords();
	}

	gameIO.End();
}