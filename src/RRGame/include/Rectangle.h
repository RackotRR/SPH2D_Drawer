#pragma once
struct Rectangle {
	int x;
	int y;
	int width;
	int height;
};

bool IsInRect(const Rectangle& rect, const struct Vector2& vec);