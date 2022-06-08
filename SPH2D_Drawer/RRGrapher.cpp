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


void RRGrapher::Show(Grid gridR, Square area, double particleSize) {
	auto& gameIO{ RRGameIO::Instance() };
	try { 
		gameIO.Initialize();

		this->grid = std::move(gridR);
		this->maxT = grid.size(); // кол-во временных слоёв 
		this->currentT = 0;
		this->currentLayer = std::begin(grid);
		this->area = area;
		this->particleSize = particleSize;
		this->passedTime = 0;
		// должен быть хотя бы один временной слой
		if (maxT == 0) {
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
	auto& layer{ *currentLayer };
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
	 
	for (auto& [x, y, type] : layer) {
		Vector2 pos{ toScreenX(x), toScreenY(y) };
		//gameIO.DrawPoint(pos, type == 2 ? realColor : virtualColor, 1); 
		gameIO.DrawRectangle(Rectangle{ pos.X, pos.Y, 2, 2 }, type == 2 ? realColor : virtualColor);
	}

	constexpr double L = 5.2915; 
	constexpr double pointX = 0.25 * L + 1.02;
	constexpr double d = 0.7;
	
	int verX = toScreenX(pointX);
	int verY = toScreenY(d);
	//gameIO.DrawLineSegment({ verX, verY - 50}, { verX, verY + 50 }, RRColor::Green());
	//gameIO.DrawLineSegment({ 0, verY }, { gameIO.GetWinWidth(), verY}, RRColor::Green());

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
		autoPlay = true;
		currentT = 0;
		currentLayer = std::begin(grid);
	}
	if (keyState.Click(RRKeyboardState::Keys::SPACE)) {
		autoPlay = !autoPlay;
	}
	if (keyState.Click(RRKeyboardState::Keys::D) || keyState.IsKeyDown(RRKeyboardState::Keys::W)) {
		auto next = currentLayer;
		next++;
		if (next != std::end(grid)) {
			currentT++;
			currentLayer++;
		}
		std::cout << "currentT: " << currentT << " / " << maxT << std::endl;
	}
	if (keyState.Click(RRKeyboardState::Keys::A) || keyState.IsKeyDown(RRKeyboardState::Keys::S)) {
		if (currentLayer != std::begin(grid)) {
			currentT--;
			currentLayer--;
		}
		std::cout << "currentT: " << currentT << " / " << maxT << std::endl;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::Z)) {
		timeToLayer -= 2;
		if (timeToLayer < 2) timeToLayer = 2;
		std::cout << "timeToLayer: " << timeToLayer << std::endl;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::X)) {
		timeToLayer += 2;
		std::cout << "timeToLayer: " << timeToLayer << std::endl;
	}

	double speed = 3;
	if (keyState.IsKeyDown(RRKeyboardState::Keys::I)) {
		deltaY -= speed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::K)) {
		deltaY += speed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::J)) {
		deltaX += speed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::L)) {
		deltaX -= speed;
	}
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
				currentT++;
				currentLayer++;
				if (currentT >= maxT) {
					currentT = 0;
					currentLayer = std::begin(grid);
					autoPlay = false;
				}
			}
		} 
		gameIO.Begin(RRColor::White());
		/* отрисовка всего и вся */  
		DrawLayer();
		gameIO.End();
	} while (!shallStop);
}