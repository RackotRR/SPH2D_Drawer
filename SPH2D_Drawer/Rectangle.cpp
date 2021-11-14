#include "Rectangle.h"
#include "Vector2.h"

bool IsInRect(const Rectangle& rect, const Vector2& vec) {
	return (rect.x < vec.X) &&
		((rect.x + rect.width) > vec.X) &&
		(rect.y < vec.Y) &&
		((rect.y + rect.height) > vec.Y);
}