#pragma once
#include "Vector2.h"

class Axis {
public:
	void Set(const Vector2& start, const Vector2& end, size_t pieces);
	 
	virtual ~Axis() = default;
protected:
	Vector2 start, end;
	size_t lines{};
};

class AxisX : public Axis {
public:
	void Draw(double scale, double delta);
};
class AxisY : public Axis {
public:
	void Draw(double scale, double delta, double bottomY);
};