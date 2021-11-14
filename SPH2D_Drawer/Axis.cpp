#include "Axis.h"
#include "RRGameIO.h"
#include <string>
#include <assert.h>

void Axis::Set(const Vector2& start, const Vector2& end, size_t lines) {
	auto& gameIO{ RRGameIO::Instance() };

	this->start = Vector2{ start.X * gameIO.GetWinWidth() / 100, start.Y * gameIO.GetWinHeight() / 100 };
	this->end = Vector2{ end.X * gameIO.GetWinWidth() / 100, end.Y * gameIO.GetWinHeight() / 100 };
	this->lines = lines;

	assert(lines > 1);
}
 


void AxisX::Draw(double scale, double delta) {
	auto& gameIO{ RRGameIO::Instance() };

	gameIO.DrawLineSegment(start, end, { 255, 255, 255 });

	int dx( (double)(end.X - start.X) / (double)(lines - 1) );

	// высота полосок
	int h( gameIO.GetWinHeight() * 0.01 );
	for (int i{}; i < lines; i++) {
		Vector2 startLine{ start.X + dx * i, start.Y - h };
		Vector2 endLine{ start.X + dx * i, start.Y + h };
		gameIO.DrawLineSegment(startLine, endLine, { 255, 255, 255 });

		double newDelta{ startLine.X - delta };
		newDelta /= scale;
		std::string numStr{ std::to_string(newDelta) };
		std::string str(numStr, 0, 5);
		Vector2 textPos{ start.X + dx * i - int(4 * numStr.size()), start.Y - 3 * h };
		gameIO.DrawLine(textPos, Font::Treasures, str);
	}

}

void AxisY::Draw(double scale, double delta, double bottomY) {
	auto& gameIO{ RRGameIO::Instance() };

	gameIO.DrawLineSegment(start, end, { 255, 255, 255 });

	int dy( (double)(end.Y - start.Y) / (double)(lines - 1) );

	// высота полосок
	int h( gameIO.GetWinHeight() * 0.01 );
	for (int i{}; i < lines; i++) {
		Vector2 startLine{ start.X - h, start.Y + dy * i };
		Vector2 endLine{ start.X + h, start.Y + dy * i };
		gameIO.DrawLineSegment(startLine, endLine, { 255, 255, 255 });

		double newDelta{ bottomY - startLine.Y - delta };
		newDelta /= scale; 

		std::string numStr{ std::to_string(newDelta) };
		std::string str(numStr, 0, 5);

		Vector2 textPos{ start.X + 2 * h, start.Y + dy * i - 8};
		gameIO.DrawLine(textPos, Font::Treasures, str);
	}

}