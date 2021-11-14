#pragma once
#include "Vector2.h"
#include "Rectangle.h"
#include "RRColor.h"

class Mouse;

class ScrollBar {
public:
	// screenRect - в % от размеров окна
	void Set(Rectangle screenRect, size_t maxValue, size_t startValue = 0);

	size_t GetValue() const {
		return value;
	}

	void Update(const Mouse& mouseState);
	void Draw();

private:
	RRColor lineColor;
	RRColor scrollColor;

	Rectangle rect; 
	Rectangle scrollRect;

	Vector2 startLine;
	Vector2 endLine;

	size_t value;
	size_t maxValue;
};

class ScrollBarV {
public:
	// screenRect - в % от размеров окна
	void Set(Rectangle screenRect, size_t maxValue, size_t startValue = 0);

	size_t GetValue() const {
		return value;
	}

	void Update(const Mouse& mouseState);
	void Draw();

private:
	RRColor lineColor;
	RRColor scrollColor;

	Rectangle rect; 
	Rectangle scrollRect;

	Vector2 startLine;
	Vector2 endLine;

	size_t value;
	size_t maxValue;
};