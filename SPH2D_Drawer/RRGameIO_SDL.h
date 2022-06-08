#pragma once
#include <SDL.h>
#include "RRGameIO.h"

class RRGameIO_SDL : public RRGameIO
{
public: 
	void Initialize() override;
	void Shutdown() override;

	void Begin(RRColor clearColor) override;
	void End() override;


	int GetWinWidth() const override;
	int GetWinHeight() const override; 

	// ���������� ��������
	void DrawTexture(const Rectangle& rect, const RRTexture& texture) override;
	// ���������� �������� � ���������
	void DrawTexture(const Rectangle& rect, const RRTexture& texture, double angle) override;
	// ���������� �������� � ��������� � ����������
	void DrawTexture(const Rectangle& rect, const RRTexture& texture, double angle, DrawFlip dflip) override;

	// ������� ������ �� �����
	void DrawLine(const Vector2& topLeft, Font font, const std::string& str) override;

	// ���������� �������������
	void DrawRectangle(const Rectangle& rect, const RRColor& color) override;

	// ���������� ������� ������
	void DrawLineSegment(const Vector2& start, const Vector2& end, const RRColor& color) override;

	void DrawPoint(const Vector2& position, const RRColor& color, int size) override;

	void DrawCircleFill(const Vector2& center, double radius, const RRColor& color) override;

	void DrawCircle(const Vector2& center, double radius, const RRColor& color) override;

	RRTexture LoadTextureFromFile(std::string_view path) override;
private: 
	bool IsInitialized{ false };

	SDL_Window* window{};
	SDL_Renderer* renderer{};

	int windowWidth{}, windowHeight{}; 
};

