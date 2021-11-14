#include <limits>
#include <algorithm>

#include "RRGrapher.h"
#include "RRGameIO.h"
#include "RRController.h"
#include "RRGameTime.h"  

RRGrapher& RRGrapher::Instance() {
	static RRGrapher instance;
	return instance;
}

void RRGrapher::Show(Grid&& gridR, Square area, double particleSize) {
	auto& gameIO{ RRGameIO::Instance() };
	try { 
		gameIO.Initialize();

		this->grid = std::move(gridR);
		this->maxT = grid.size(); // кол-во временных слоЄв 
		this->area = area;
		this->particleSize = particleSize;
		// должен быть хот€ бы один временной слой
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
	auto& layer{ grid[currentT] };
	auto& gameIO{ RRGameIO::Instance() };
	  
	// Ќарисовать границы квадрата:
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

	// top
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x0), toScreenY(y0) },
		Vector2{ toScreenX(x1), toScreenY(y0) }, 
		RRColor::White());
	// bot
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x0), toScreenY(y1) },
		Vector2{ toScreenX(x1), toScreenY(y1) }, 
		RRColor::White());
	// left
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x0), toScreenY(y0) },
		Vector2{toScreenX(x0), toScreenY(y1)},
		RRColor::White()
	);
	// right
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x1), toScreenY(y0) },
		Vector2{toScreenX(x1), toScreenY(y1)},
		RRColor::White()
	);
	 
	// рисуем круги
	for (auto& [x, y] : layer) {
		Vector2 pos{ toScreenX(x), toScreenY(y) };
		//gameIO.DrawPoint(pos, RRColor::Blue());
		gameIO.DrawCircleFill(pos, 7, RRColor::Blue());
	}
	//// –исовка точками
	//for (auto& [x, y] : layer) {
	//	Vector2 pos{ toScreenX(x), toScreenY(y) };
	//	 
	//	gameIO.DrawPoint(pos, RRColor::Blue());
	//}

	// –исовка лини€ми
	//for (size_t i{ 1 }; i < layer.size(); i++) {
	//	double x0{ startX + (i - 1) * dx };
	//	double x1{ startX + i * dx };
	//	double y0{ layer[i - 1] };
	//	double y1{ layer[i] };
	//	Vector2 start{ int(deltaX + x0 * scaleX), bottomY - int(deltaY + y0 * scaleY) };
	//	Vector2 end{ int(deltaX + x1 * scaleX), bottomY - int(deltaY + y1 * scaleY) };
	//	gameIO.DrawLineSegment(start, end, RRColor{ 255, 255, 255 });
	//}

	//// –исовка точками
	//for (size_t i{ 0 }; i < layer.size(); i++) { 
	//	double x1{ startX + i * dx }; 
	//	double y1{ layer[i] }; 
	//	
	//	Vector2 pos{ int(deltaX + x1 * scaleX), bottomY - int(deltaY + y1 * scaleY) };
	//	gameIO.DrawPoint(pos, RRColor{ 255, 255, 255 });
	//}
}


void RRGrapher::ComputeStartScale() { 
	auto& gameIO{ RRGameIO::Instance() };
	int startX{};
	int startY{};
	int endX{ gameIO.GetWinWidth() };
	int endY{ static_cast<int>(gameIO.GetWinHeight() * 0.8) };

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

	timeScroll.Set({ 3, 94, 80, 6 }, maxT - 1); 
}

void RRGrapher::Stop() {
	shallStop = true;
}

void RRGrapher::RunWindowCycle() { 
	auto& controller{ RRController::Instance() };
	auto& gameTime{ RRGameTime::Instance() };
	auto& gameIO{ RRGameIO::Instance() };
	 

	controller.Update();
	do {
		controller.Update();
		gameTime.Update();
		/* обработка игровой логики */ 
		if (controller.HasQuitEvent()) {
			Stop();
		}
		 
		   
		auto& mouseState{ controller.GetMouseState() };
		if (mouseState.RightButton == Mouse::State::Pressed) {
			Vector2 deltaPos{ mouseState.Position - mouseState.PrevPosition };
			deltaX += deltaPos.X;
			deltaY -= deltaPos.Y;
		}

		auto& keyState{ controller.GetKeyState() };
		if (keyState.IsKeyDown(RRKeyboardState::Keys::Q)) {
			scaleCoord *= 1.1;
		}
		if (keyState.IsKeyDown(RRKeyboardState::Keys::E)) {
			scaleCoord *= 0.9;
		}
		if (keyState.Click(RRKeyboardState::Keys::ENTER)) {
			autoPlay = true;
			currentT = 0;
		}


		if (autoPlay) {
			passedTime += gameTime.GetPassedTime();
			if (passedTime > timeToLayer) {
				passedTime = 0;
				currentT++;
				if (currentT >= maxT) {
					currentT = 0; 
					autoPlay = false;
				}
			}
		}
		else {
			timeScroll.Update(controller.GetMouseState());
			currentT = timeScroll.GetValue();
		}
		gameIO.Begin();
		/* отрисовка всего и вс€ */  
		DrawLayer();
		//timeScroll.Draw();  
		gameIO.End();
	} while (!shallStop);
}