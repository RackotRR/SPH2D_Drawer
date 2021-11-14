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

	// ���������������� ���� �������� ����� GameIO
	void Initialize(SDL_Renderer* renderer);

	// ������� ��� ������������ ������
	void Shutdown();

	// ������� �� ����� ������ ��������� �������
	void DrawLine(const Vector2& topleft, Font font, std::string_view str);
private:
	// �������� �� �������� ���������� ������ ��������
	const RRTexture& GetTextureFrom(Font font, const char& c);

	// ��������� ����� � ��������� �����������
	void LoadFont(Font key, const std::string& path, int size, const RRColor& color, int styleFlags = 0);

	// �������������� ��� ������. �������� ����� �� ��������������
	/*
	����� ������ ��������� �����!!!
	*/
	void InitFonts();

	// �������� ��� ������ ������� � ���� �������
	void RenderFont(Font font, const RRColor& c);

	SDL_Renderer* renderer;
	std::unordered_map<Font, TTF_Font*> fonts;
	std::unordered_map<Font, std::unordered_map<char, RRTexture>> renderedFonts;

	RRTextOutput() : renderer{} {}
	~RRTextOutput() {}

	RRTextOutput(const RRTextOutput&) = delete;
	RRTextOutput& operator=(const RRTextOutput) = delete;
};

