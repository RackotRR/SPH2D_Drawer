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
		this->currentT = 0;
		this->area = area;
		this->particleSize = particleSize;
		this->passedTime = 0;
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
		RRColor::Black());
	// bot
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x0), toScreenY(y1) },
		Vector2{ toScreenX(x1), toScreenY(y1) }, 
		RRColor::Black());
	// left
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x0), toScreenY(y0) },
		Vector2{ toScreenX(x0), toScreenY(y1) },
		RRColor::Black()
	);
	// right
	gameIO.DrawLineSegment(
		Vector2{ toScreenX(x1), toScreenY(y0) },
		Vector2{ toScreenX(x1), toScreenY(y1) },
		RRColor::Black()
	);

	//double nBlocksX( size.first / (particleSize * 3) );
	//double nBlocksY( size.second / (particleSize * 3) );
	//double blockSize( particleSize * 3 );
	//for (size_t i{}; i < nBlocksX; i++) {
	//	gameIO.DrawLineSegment(
	//		Vector2{ toScreenX(x0 + blockSize * i), toScreenY(y0) },
	//		Vector2{ toScreenX(x0 + blockSize * i), toScreenY(y1) },
	//		RRColor::White());
	//}
	//for (size_t i{}; i < nBlocksY; i++) {
	//	gameIO.DrawLineSegment(
	//		Vector2{ toScreenX(x0), toScreenY(y0 + blockSize * i) },
	//		Vector2{ toScreenX(x1), toScreenY(y0 + blockSize * i) },
	//		RRColor::White());
	//}

	//size_t sizeL{ layer.size() };
	//size_t index = sizeL / 6;
	//auto iter = layer.begin();
	//for (size_t i{}; i < index; i++) {
	//	iter++;
	//}
	//auto& [xc, yc] = *iter;
	//gameIO.DrawCircle(Vector2{ toScreenX(xc), toScreenY(yc) }, particleSize * 3 * scaleCoord, RRColor::Black());

	constexpr RRColor realColor = RRColor::Blue();
	constexpr RRColor virtualColor = RRColor::Red();
	 
	// рисуем круги
	for (auto& [x, y, type] : layer) {
		Vector2 pos{ toScreenX(x), toScreenY(y) };
		gameIO.DrawPoint(pos, type == 2 ? realColor : virtualColor); 
		//if (x == xc && y == yc) {
		//	gameIO.DrawCircleFill(pos, 7, RRColor::Black());
		//}
		// else { 
		//gameIO.DrawCircle(Vector2{ toScreenX(x), toScreenY(y) }, particleSize * scaleCoord, RRColor::Blue());
			//gameIO.DrawCircle(pos, 5, RRColor::Blue());
		// }
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
		if (keyState.Click(RRKeyboardState::Keys::Q)) {
			scaleCoord *= 1.1;
		}
		if (keyState.Click(RRKeyboardState::Keys::E)) {
			scaleCoord *= 0.9;
		}
		if (keyState.Click(RRKeyboardState::Keys::ENTER)) {
			autoPlay = true;
			currentT = 0;
		}
		if (keyState.Click(RRKeyboardState::Keys::SPACE)) {
			autoPlay = !autoPlay;
		}
		if (keyState.Click(RRKeyboardState::Keys::D) || keyState.IsKeyDown(RRKeyboardState::Keys::W)) {
			if (currentT < maxT - 1)
				currentT++;
			std::cout << "currentT: " << currentT << " / " << maxT << std::endl;
		}
		if (keyState.Click(RRKeyboardState::Keys::A) || keyState.IsKeyDown(RRKeyboardState::Keys::S)) {
			if (currentT > 0) {
				currentT--;
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
		gameIO.Begin(RRColor::Black());
		/* отрисовка всего и вс€ */  
		DrawLayer();
		//timeScroll.Draw();  
		gameIO.End();
	} while (!shallStop);
}