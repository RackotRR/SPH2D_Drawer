#include "RRGameIO_SDL.h"
#include "RRTexture.h"
#include "RRTextOutput.h"
#include "RRTextureManager.h"
#include "RRGameTime.h"

#include <SDL_image.h>
#include <stdexcept>
#include <cmath>
#include <iostream>

RRGameIO& RRGameIO::Instance() {
	static RRGameIO_SDL instance;
	return instance;
} 

void RRGameIO_SDL::Initialize() {
	assert(IsInitialized == false);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		throw std::runtime_error{ "Unable to initialize SDL: " + std::string{ SDL_GetError() } };
	}

	SDL_Rect displayBounds;
	if (SDL_GetDisplayBounds(0, &displayBounds) != 0) { 
		throw std::runtime_error{ "SDL_GetDisplayBounds failed: " + std::string{ SDL_GetError() } };
	}
	else {
		windowHeight = static_cast<int>( displayBounds.h * 0.8 );
		windowWidth = static_cast<int>( displayBounds.w * 0.8 );
	}

	window = SDL_CreateWindow("SPH2D_Drawer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		windowWidth, windowHeight, 0);
	if (!window) { 
		throw std::runtime_error{ "Failed to create window: " + std::string{ SDL_GetError() } };
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		throw std::runtime_error{ "Failed to create renderer: " + std::string{ SDL_GetError() } };
	}

	// режим прозрачности
	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0) { 
		throw std::runtime_error{ "Failed to set render draw blend mode: " + std::string{ SDL_GetError() } };
	}

	// модуль для загрузки текстур в PNG
	if (IMG_Init(IMG_INIT_PNG) == 0) { 
		throw std::runtime_error{ "Unable to initialize SDL_image: " + std::string{ SDL_GetError() } };
	}

	IsInitialized = true;

	// загрузка менеджера текстур
	RRTextureManager::Instance().Initialize();
	 
	// модуль для вывода генерации текстур из текста
	RRTextOutput::Instance().Initialize(renderer);

}

void RRGameIO_SDL::Shutdown() {
	RRTextOutput::Instance().Shutdown();
	RRTextureManager::Instance().Shutdown();
	IMG_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void RRGameIO_SDL::Begin(RRColor clearColor) {
	assert(IsInitialized);
	auto& [r, g, b, a] = clearColor;

	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);
}
void RRGameIO_SDL::End() {
	assert(IsInitialized); 

	SDL_RenderPresent(renderer);
}
  
int RRGameIO_SDL::GetWinWidth() const {
	assert(IsInitialized);

	return windowWidth;
}
int RRGameIO_SDL::GetWinHeight() const {
	assert(IsInitialized);

	return windowHeight;
}
  

// нарисовать текстуру
void RRGameIO_SDL::DrawTexture(const Rectangle& rect, const RRTexture& texture) {
	assert(IsInitialized);

	DrawTexture(rect, texture, 0, DrawFlip::none);
}
// нарисовать текстуру с поворотом
void RRGameIO_SDL::DrawTexture(const Rectangle& rect, const RRTexture& texture, double angle) {
	assert(IsInitialized);

	DrawTexture(rect, texture, angle, DrawFlip::none);
}
// нарисовать текстуру с поворотом и отражением
void RRGameIO_SDL::DrawTexture(const Rectangle& rect, const RRTexture& texture, double angle, DrawFlip dflip) {
	assert(IsInitialized);

	SDL_Rect r{ rect.x, rect.y, rect.width, rect.height }; // где мы будем рисовать

	SDL_RendererFlip flip; // отражение
	switch (dflip)
	{
	case DrawFlip::horizontal:
		flip = SDL_FLIP_HORIZONTAL;
		break;
	case DrawFlip::vertical:
		flip = SDL_FLIP_VERTICAL;
		break;
	default:
		flip = SDL_FLIP_NONE;
		break;
	}

	// рисуем
	SDL_RenderCopyEx(renderer,
		texture.Get(), // текстурка
		nullptr, // вся текстура
		&r, // где
		angle, // угол поворота
		nullptr, // относительно центра
		flip); // отражение
}

// вывести строку на экран
void RRGameIO_SDL::DrawLine(const Vector2& topLeft, Font font, const std::string& str) {
	assert(IsInitialized);

	RRTextOutput::Instance().DrawLine(topLeft, font, str);
} 

// нарисовать прямоугольник
void RRGameIO_SDL::DrawRectangle(const Rectangle& rect, const RRColor& color) {
	assert(IsInitialized);

	SDL_Rect r{ rect.x, rect.y, rect.width, rect.height };
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &r); 
}


void RRGameIO_SDL::DrawLineSegment(const Vector2& start, const Vector2& end, const RRColor& color) {
	assert(IsInitialized);

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(renderer, start.X, start.Y, end.X, end.Y);
}


void RRGameIO_SDL::DrawPoint(const Vector2& position, const RRColor& color, int size) {
	assert(IsInitialized);
	 
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(renderer, position.X - size, position.Y, position.X + size, position.Y);
	SDL_RenderDrawLine(renderer, position.X, position.Y - size, position.X, position.Y + size);
}

void RRGameIO_SDL::DrawCircleFill(const Vector2& center, double radius, const RRColor& color) {
	assert(IsInitialized);
	 
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	
	double deltaAngle{ 1.0 / radius};
	if (deltaAngle <= 0) {
		throw std::runtime_error{ "DrawCirlceFill: Wrong delta angle" };
	}

	for (double angle{}; angle < M_PI; angle += deltaAngle) {
		int x{ static_cast<int>(radius * cos(angle)) };
		int y{ static_cast<int>(radius * sin(angle)) }; 
		SDL_RenderDrawLine(renderer, center.X + x, center.Y + y, center.X + x, center.Y - y); 
	}
}

void RRGameIO_SDL::DrawCircle(const Vector2& center, double radius, const RRColor& color) {
	assert(IsInitialized);
	 
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	
	double deltaAngle{ 1.0 / radius};
	if (deltaAngle <= 0) {
		throw std::runtime_error{ "DrawCirlceFill: Wrong delta angle" };
	}

	for (double angle{}; angle < M_PI; angle += deltaAngle) {
		int x{ static_cast<int>(radius * cos(angle)) };
		int y{ static_cast<int>(radius * sin(angle)) }; 
		//SDL_RenderDrawLine(renderer, center.X + x, center.Y + y, center.X + x, center.Y - y); 
		SDL_RenderDrawPoint(renderer, center.X + x, center.Y + y);
		SDL_RenderDrawPoint(renderer, center.X + x, center.Y - y);
	}
}


RRTexture RRGameIO_SDL::LoadTextureFromFile(std::string_view path) {
	assert(IsInitialized);

	SDL_Surface* surface{ IMG_Load(path.data()) };
	if (!surface) { 
		throw std::runtime_error{ "Failed to load texture file: " + std::string{ path } };
	}

	SDL_Texture* texture{ SDL_CreateTextureFromSurface(renderer, surface) };
	SDL_FreeSurface(surface);
	if (!texture) { 
		throw std::runtime_error{ "Failed to convert surface to texture for " + std::string{ path } };
	}

	return RRTexture{ texture };
}


void RRGameIO_SDL::MakeScreenshot(const std::string& path) {
	assert(IsInitialized);
	
	SDL_Surface* sshot = SDL_CreateRGBSurface(0, GetWinWidth(), GetWinHeight(), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	IMG_SavePNG(sshot, (path + ".png").c_str());
	SDL_FreeSurface(sshot);
}