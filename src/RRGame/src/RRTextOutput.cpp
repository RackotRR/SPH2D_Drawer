#include "RRTextOutput.h"
#include <limits>
#include <stdexcept>
#include <filesystem>

RRTextOutput& RRTextOutput::Instance() {
	static RRTextOutput instance;
	return instance;
}

void RRTextOutput::Initialize(SDL_Renderer* renderer) { 
	// инициализируем  
	if (!TTF_WasInit() && TTF_Init() == -1) {  
		throw std::runtime_error{ "TTF_Init: " + std::string{ TTF_GetError() } };
	}

	this->renderer = renderer;

	// инициализируем шрифты
	InitFonts();
}

void RRTextOutput::Shutdown() {
	for (auto& iter : renderedFonts) { // iter - указатель на отображение char в RRTexture
		for (auto& i : iter.second) { // i - пара char и RRTexture
			i.second.Destroy(); // освобождаем память в RRTexture 
		} 
	} 
	// закрываем SDL_ttf
	if (TTF_WasInit())
		TTF_Quit();
}

void RRTextOutput::InitFonts() {
	auto fonts_dir = std::filesystem::path("fonts");
	auto font = fonts_dir / "FreeMono.ttf";
	LoadFont(Font::Menu, font.string(), 24, RRColor::Black(), TTF_STYLE_BOLD);
	/*
	добавялем здесь новые шрифты
	*/ 
}

void RRTextOutput::LoadFont(Font key, const std::string& path, int size, const RRColor& color, int styleFlags) {
	TTF_Font* font;
	// загружаем шрифт
	font = TTF_OpenFont(path.c_str(), size);
	if (!font) { // если не получилось 
		throw std::runtime_error{ "TTF_OpenFont: " + std::string{ TTF_GetError() } };
	}

	// добавляем стили
	if (styleFlags != TTF_STYLE_NORMAL) {
		TTF_SetFontStyle(font, styleFlags);
	}

	// добавляем в список шрифтов
	fonts[key] = font;
	RenderFont(key, color);
}

void RRTextOutput::RenderFont(Font font, const RRColor& c) {
	char glyph[2]; // си-строка вида "?\0"
	glyph[1] = '\0'; // входная си-строка должна заканчиваться \0

	SDL_Color color{ c.r, c.g, c.b, c.a }; // white 

	SDL_Surface* surface;
	SDL_Texture* texture; 
	std::unordered_map<char, RRTexture> textureMap;

	// рендерим английский алфавит и знаки
	for (char i{ 32 }; i < 127; i++) {
		surface = nullptr;
		texture = nullptr; 

		glyph[0] = i; // символ для рендера

		// пытаемся отрендерить текст на surface
		surface = TTF_RenderUTF8_Blended(fonts[font], glyph, color);
		if (surface == nullptr) { 
			throw std::runtime_error{ "TTF_RenderUTF8_Blended fault: " + std::string{ TTF_GetError() } };
		}

		// пытаемся сделать из surface текстуру для дальнейшего использования
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == nullptr) { 
			throw std::runtime_error{ "SDL_CreateTextureFromSurface fault: " + std::string{ TTF_GetError() } };
		}
		 
		// оборачиваем текстуру в RRTexture и добавляем в массив текстур
		textureMap[i] = RRTexture{ texture };
	}
	// добавляем полученный массив в общий
	renderedFonts[font] = textureMap; 
}

const RRTexture& RRTextOutput::GetTextureFrom(Font font, const char& c) {
	// получаем текстуру для заданного символа
	return renderedFonts[font].at(c);
}

void RRTextOutput::DrawLine(const Vector2& topleft, Font font, std::string_view str) { 
	// посимвольно проходим по строке и рендерим каждый символ
	int x = topleft.X;
	int y = topleft.Y;
	for (int i = 0; i < str.size(); i++) {
		const RRTexture& texture{ GetTextureFrom(font, str[i]) };
		Vector2 size{ texture.GetSize() };
		x += size.X;
		SDL_Rect rect{ x, topleft.Y, size.X, size.Y };
		SDL_RenderCopy(renderer, texture.Get(), nullptr, &rect);
	}
}