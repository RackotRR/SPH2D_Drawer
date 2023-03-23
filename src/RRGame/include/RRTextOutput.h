#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Vector2.h"
#include "RRColor.h" 
#include "RRTexture.h"
#include "RRGameIO.h"
#include <unordered_map>
#include <string>

class RRTextOutput
{
public:
	static RRTextOutput& Instance();

	// инициализировать этот синглтон будет GameIO
	void Initialize(SDL_Renderer* renderer);

	// вызвать для освобождения памяти
	void Shutdown();

	// вывести на экран строку указанным шрифтом
	void DrawLine(const Vector2& topleft, Font font, std::string_view str);
private:
	// получает из большого контейнера нужную текстуру
	const RRTexture& GetTextureFrom(Font font, const char& c);

	// загружает шрифт с заданными параметрами
	void LoadFont(Font key, const std::string& path, int size, const RRColor& color, int styleFlags = 0);

	// инициализирует все шрифты. вызывать перед их использованием
	/*
	новые шрифты добавлять здесь!!!
	*/
	void InitFonts();

	// рендерит все нужные символы с этим шрифтом
	void RenderFont(Font font, const RRColor& c);

	SDL_Renderer* renderer;
	std::unordered_map<Font, TTF_Font*> fonts;
	std::unordered_map<Font, std::unordered_map<char, RRTexture>> renderedFonts;

	RRTextOutput() : renderer{} {}
	~RRTextOutput() {}

	RRTextOutput(const RRTextOutput&) = delete;
	RRTextOutput& operator=(const RRTextOutput) = delete;
};

