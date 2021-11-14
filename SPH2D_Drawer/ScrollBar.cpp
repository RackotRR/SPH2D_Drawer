#include <assert.h>

#include "ScrollBar.h"
#include "Mouse.h"
#include "RRGameIO.h"


void ScrollBar::Set(Rectangle screenRect, size_t maxValue, size_t startValue) {
	assert(screenRect.width >= 0 && screenRect.width <= 100);
	assert(screenRect.height >= 0 && screenRect.height <= 100);

	lineColor = { 255, 255, 255 };
	scrollColor = { 255, 255, 255 };
	value = startValue;
	this->maxValue = maxValue;

	// screenRect [%] -> rect [px]
	auto& gameIO{ RRGameIO::Instance() };

	// размеры элемента управления
	rect.x = gameIO.GetWinWidth() * screenRect.x / 100;
	rect.y = gameIO.GetWinHeight() * screenRect.y / 100;
	rect.width = gameIO.GetWinWidth() * screenRect.width / 100;
	rect.height = gameIO.GetWinHeight() * screenRect.height / 100; 

	// размеры scroll 
	scrollRect.y = rect.y;
	scrollRect.width = rect.height / 3;
	scrollRect.height = rect.height;

	// размеры line
	startLine = { rect.x, rect.y + rect.height / 2 };
	endLine = { rect.x + rect.width, rect.y + rect.height / 2 };
}

void ScrollBar::Draw() {
	auto& gameIO{ RRGameIO::Instance() };
	 
	// draw line
	gameIO.DrawLineSegment(startLine, endLine, lineColor); 
	 
	// draw scroll
	double percentValue{ (double)value / (double)maxValue };
	scrollRect.x = rect.x + rect.width * percentValue - scrollRect.width / 2; 
	gameIO.DrawRectangle(scrollRect, scrollColor);
}

void ScrollBar::Update(const Mouse& mouseState) {
	if (mouseState.LeftButton == Mouse::State::Pressed
		&& IsInRect(Rectangle{ rect.x - 5, rect.y - 5, rect.width + 10, rect.height + 10 }, mouseState.Position))
	{
		int mouseX{ mouseState.Position.X };
		double percentValue{ (double)(mouseX - rect.x) / (double)rect.width };
		if (percentValue < 0) percentValue = 0;
		else if (percentValue > 1) percentValue = 1;
		value = maxValue * percentValue;
	}
}


void ScrollBarV::Set(Rectangle screenRect, size_t maxValue, size_t startValue) {
	assert(screenRect.width >= 0 && screenRect.width <= 100);
	assert(screenRect.height >= 0 && screenRect.height <= 100);

	lineColor = { 255, 255, 255 };
	scrollColor = { 255, 255, 255 };
	value = startValue;
	this->maxValue = maxValue;

	// screenRect [%] -> rect [px]
	auto& gameIO{ RRGameIO::Instance() };

	// размеры элемента управления
	rect.x = gameIO.GetWinWidth() * screenRect.x / 100;
	rect.y = gameIO.GetWinHeight() * screenRect.y / 100;
	rect.width = gameIO.GetWinWidth() * screenRect.width / 100;
	rect.height = gameIO.GetWinHeight() * screenRect.height / 100; 

	// размеры scroll 
	scrollRect.x = rect.x;
	scrollRect.width = rect.width;
	scrollRect.height = rect.width / 3;

	// размеры line
	startLine = { rect.x + rect.width / 2, rect.y };
	endLine = { rect.x + rect.width / 2, rect.y + rect.height };
}

void ScrollBarV::Draw() {
	auto& gameIO{ RRGameIO::Instance() };
	 
	// draw line
	gameIO.DrawLineSegment(startLine, endLine, lineColor); 
	 
	// draw scroll
	double percentValue{ (double)value / (double)maxValue };
	scrollRect.y = rect.y + rect.height * percentValue - scrollRect.height / 2; 
	gameIO.DrawRectangle(scrollRect, scrollColor);
}

void ScrollBarV::Update(const Mouse& mouseState) {
	if (mouseState.LeftButton == Mouse::State::Pressed &&
		IsInRect(rect, mouseState.Position)) 
	{
		int mouseY{ mouseState.Position.Y };
		double percentValue{ (double)(mouseY - rect.y) / (double)rect.height };
		value = maxValue * percentValue;
	}
}