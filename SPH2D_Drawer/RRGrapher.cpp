#include <limits>
#include <algorithm>
#include <iostream>

#include "RRGrapher.h"
#include "RRGameIO.h"
#include "RRController.h"
#include "RRGameTime.h"  

RRGrapher& RRGrapher::Instance() {
	static RRGrapher instance;
	return instance;
}

void RRGrapher::SetupHeatMap(double min, double max) {
	heatMap.SetNew(min, max);
}

void RRGrapher::Show(Grid gridR, Square area, double particleSize) {
	auto& gameIO{ RRGameIO::Instance() };
	try { 
		gameIO.Initialize();
		InitConsoleCommands();

		this->grid = std::move(gridR);
		this->currentLayer = 0;
		this->area = area;
		this->particleSize = particleSize;
		this->passedTime = 0;
		// должен быть хотя бы один временной слой
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

	// Нарисовать границы квадрата:
	const auto& [origin, size] = area;
	double x0{ origin.first };
	double x1{ origin.first + size.first };
	double y0{ origin.second };
	double y1{ origin.second + size.second };

	double bottomY{ gameIO.GetWinHeight() * 0.8 };

	auto toScreenX{ 
		[=](double x) -> int {
			return deltaX + x * scaleCoord;
		}
	};
	auto toScreenY{
		[=](double y) -> int {
			return bottomY - (deltaY + y * scaleCoord);
		}
	};

	constexpr RRColor realColor = RRColor::Blue();
	constexpr RRColor virtualColor = RRColor::Black();
	 
	for (auto& [x, y, type, vx, vy, p, rho, u] : layer) {
		Vector2 pos{ toScreenX(x), toScreenY(y) };
		//gameIO.DrawPoint(pos, type == 2 ? realColor : virtualColor, 1); 
		//gameIO.DrawRectangle(Rectangle{ pos.X, pos.Y, 3, 3 }, type == 2 ? realColor : virtualColor);
		gameIO.DrawRectangle(Rectangle{ pos.X, pos.Y, 
			1 + int(particleSize * scaleCoord), 
			1+ int(particleSize * scaleCoord) }, 
			heatMap.GetNewColorForNum(p));
	}

	constexpr double L = 5.2915; 
	constexpr double pointX = 0.25 * L + 1.02;
	constexpr double d = 0.7;
	
	int verX = toScreenX(pointX);
	int verY = toScreenY(d);
	//gameIO.DrawLineSegment({ verX, verY - 50}, { verX, verY + 50 }, RRColor::Green());
	//gameIO.DrawLineSegment({ 0, verY }, { gameIO.GetWinWidth(), verY}, RRColor::Green());

}


void RRGrapher::DrawLegend() const {
	auto legend = heatMap.GetLegend();

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
	gameIO.DrawLine({ int(w * 0.7), 0 }, Font::Menu, "p");
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

void RRGrapher::UpdateControls() {
	auto& controller{ RRController::Instance() };

	auto& mouseState{ controller.GetMouseState() };
	if (mouseState.RightButton == Mouse::State::Pressed) {
		Vector2 deltaPos{ mouseState.Position - mouseState.PrevPosition };
		deltaX += deltaPos.X;
		deltaY -= deltaPos.Y;
	}

	auto& keyState{ controller.GetKeyState() };
	if (keyState.Click(RRKeyboardState::Keys::Q)) {
		scaleCoord *= 1.1;
	}
	if (keyState.Click(RRKeyboardState::Keys::E)) {
		scaleCoord *= 0.9;
	}

	if (keyState.Click(RRKeyboardState::Keys::ENTER)) {
		autoPlay = false;
		currentLayer = 0;
	}
	if (keyState.Click(RRKeyboardState::Keys::SPACE)) {
		autoPlay = !autoPlay;
		if (currentLayer == grid.size() - 1) {
			currentLayer = 0;
		}
	}

	if (keyState.Click(RRKeyboardState::Keys::D) || keyState.IsKeyDown(RRKeyboardState::Keys::W)) {
		if (currentLayer < grid.size() - 1) {
			currentLayer++;
		}
		std::cout << "currentT: " << currentLayer << " / " << grid.size() << std::endl;
	}
	if (keyState.Click(RRKeyboardState::Keys::A) || keyState.IsKeyDown(RRKeyboardState::Keys::S)) {
		if (currentLayer > 0) {
			currentLayer--;
		}
		std::cout << "currentT: " << currentLayer << " / " << grid.size() << std::endl;
	}

	if (keyState.IsKeyDown(RRKeyboardState::Keys::Z)) {
		timeToLayer -= 2;
		if (timeToLayer < 2) timeToLayer = 1;
		std::cout << "timeToLayer: " << timeToLayer << std::endl;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::X)) {
		timeToLayer += 2;
		std::cout << "timeToLayer: " << timeToLayer << std::endl;
	}

	if (keyState.IsKeyDown(RRKeyboardState::Keys::I)) {
		deltaY -= spaceSpeed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::K)) {
		deltaY += spaceSpeed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::J)) {
		deltaX += spaceSpeed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::L)) {
		deltaX -= spaceSpeed;
	}

	if (keyState.Click(RRKeyboardState::Keys::F1)) {
		UpdateConsoleInput();
	}
}

void RRGrapher::UpdateConsoleInput() {
	std::string command;
	std::cout << "print \"exit\" to finish console input" << std::endl;
	autoPlay = false;

	while (std::cout << ">> ",
		std::cin >> command,
		command != "exit")
	{
		if (commands.contains(command)) {
			auto& response = commands[command];
			response();
			UpdateDraw();
		}
		else {
			std::cout << "unqnown command" << std::endl;
		}
	}
}
void RRGrapher::InitConsoleCommands() {
	commands.emplace("set", [&] {
		std::string var_name;
		std::cin >> var_name;

		if (var_name == "help") {
#define POSSIBLE_VARIABLE_TO_SET(variable) std::cout << typeid(variable).name() << " " << #variable << " = " << variable << std::endl
			std::cout << "possible variables to set:" << std::endl;
			POSSIBLE_VARIABLE_TO_SET(timeToLayer);
			POSSIBLE_VARIABLE_TO_SET(scaleCoord);
			POSSIBLE_VARIABLE_TO_SET(spaceSpeed);
			POSSIBLE_VARIABLE_TO_SET(deltaX);
			POSSIBLE_VARIABLE_TO_SET(deltaY);
			POSSIBLE_VARIABLE_TO_SET(particleSize);
			POSSIBLE_VARIABLE_TO_SET(currentLayer);
#undef POSSIBLE_VARIABLE_TO_SET
		}

#define COMMAND_SET_VARIABLE(variable) if (var_name == #variable) { std::cin >> variable; return; }
		COMMAND_SET_VARIABLE(timeToLayer);
		COMMAND_SET_VARIABLE(scaleCoord);
		COMMAND_SET_VARIABLE(spaceSpeed);
		COMMAND_SET_VARIABLE(deltaX);
		COMMAND_SET_VARIABLE(deltaY);
		COMMAND_SET_VARIABLE(particleSize);
		COMMAND_SET_VARIABLE(currentLayer);
#undef COMMAND_SET_VARIABLE
	});

	commands.emplace("help", [&] {
		std::cout << "commands: " << std::endl;
		for (auto& [command, func] : commands) {
			std::cout << command << std::endl;
		}
	});
}

void RRGrapher::RunWindowCycle() { 
	auto& controller{ RRController::Instance() };
	auto& gameTime{ RRGameTime::Instance() };
	auto& gameIO{ RRGameIO::Instance() };
	 
	do {
		controller.Update();
		gameTime.Update();
		/* обработка игровой логики */ 
		if (controller.HasQuitEvent()) {
			Stop();
		}
		 
		UpdateControls();
		
		if (autoPlay) {
			passedTime += gameTime.GetPassedTime();
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

		UpdateDraw();
	} while (!shallStop);
}
void RRGrapher::UpdateDraw() const {
	auto& gameIO{ RRGameIO::Instance() };
	gameIO.Begin(RRColor::White());
	DrawLayer();
	DrawLegend();
	gameIO.End();
}