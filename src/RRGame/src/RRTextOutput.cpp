#include "RRTextOutput.h"
#include <limits>
#include <stdexcept>
#include <filesystem>

RRTextOutput& RRTextOutput::Instance() {
	static RRTextOutput instance;
	return instance;
}

void RRTextOutput::Initialize(SDL_Renderer* renderer) { 
	// ��������������  
	if (!TTF_WasInit() && TTF_Init() == -1) {  
		throw std::runtime_error{ "TTF_Init: " + std::string{ TTF_GetError() } };
	}

	this->renderer = renderer;

	// �������������� ������
	InitFonts();
}

void RRTextOutput::Shutdown() {
	for (auto& iter : renderedFonts) { // iter - ��������� �� ����������� char � RRTexture
		for (auto& i : iter.second) { // i - ���� char � RRTexture
			i.second.Destroy(); // ����������� ������ � RRTexture 
		} 
	} 
	// ��������� SDL_ttf
	if (TTF_WasInit())
		TTF_Quit();
}

void RRTextOutput::InitFonts() {
	auto fonts_dir = std::filesystem::path("fonts");
	auto font = fonts_dir / "FreeMono.ttf";
	LoadFont(Font::Menu, font.string(), 24, RRColor::Black(), TTF_STYLE_BOLD);
	/*
	��������� ����� ����� ������
	*/ 
}

void RRTextOutput::LoadFont(Font key, const std::string& path, int size, const RRColor& color, int styleFlags) {
	TTF_Font* font;
	// ��������� �����
	font = TTF_OpenFont(path.c_str(), size);
	if (!font) { // ���� �� ���������� 
		throw std::runtime_error{ "TTF_OpenFont: " + std::string{ TTF_GetError() } };
	}

	// ��������� �����
	if (styleFlags != TTF_STYLE_NORMAL) {
		TTF_SetFontStyle(font, styleFlags);
	}

	// ��������� � ������ �������
	fonts[key] = font;
	RenderFont(key, color);
}

void RRTextOutput::RenderFont(Font font, const RRColor& c) {
	char glyph[2]; // ��-������ ���� "?\0"
	glyph[1] = '\0'; // ������� ��-������ ������ ������������� \0

	SDL_Color color{ c.r, c.g, c.b, c.a }; // white 

	SDL_Surface* surface;
	SDL_Texture* texture; 
	std::unordered_map<char, RRTexture> textureMap;

	// �������� ���������� ������� � �����
	for (char i{ 32 }; i < 127; i++) {
		surface = nullptr;
		texture = nullptr; 

		glyph[0] = i; // ������ ��� �������

		// �������� ����������� ����� �� surface
		surface = TTF_RenderUTF8_Blended(fonts[font], glyph, color);
		if (surface == nullptr) { 
			throw std::runtime_error{ "TTF_RenderUTF8_Blended fault: " + std::string{ TTF_GetError() } };
		}

		// �������� ������� �� surface �������� ��� ����������� �������������
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == nullptr) { 
			throw std::runtime_error{ "SDL_CreateTextureFromSurface fault: " + std::string{ TTF_GetError() } };
		}
		 
		// ����������� �������� � RRTexture � ��������� � ������ �������
		textureMap[i] = RRTexture{ texture };
	}
	// ��������� ���������� ������ � �����
	renderedFonts[font] = textureMap; 
}

const RRTexture& RRTextOutput::GetTextureFrom(Font font, const char& c) {
	// �������� �������� ��� ��������� �������
	return renderedFonts[font].at(c);
}

void RRTextOutput::DrawLine(const Vector2& topleft, Font font, std::string_view str) { 
	// ����������� �������� �� ������ � �������� ������ ������
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