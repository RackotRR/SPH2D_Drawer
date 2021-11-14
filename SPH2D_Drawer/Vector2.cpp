#include "Vector2.h"

bool operator==(const Vector2& v1, const Vector2& v2) {
	return v1.X == v2.X && v1.Y == v2.Y;
}
bool operator!=(const Vector2& v1, const Vector2& v2) {
	return !(v1 == v2);
}


Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	Vector2 v;
	v.X = v1.X + v2.X;
	v.Y = v1.Y + v2.Y;
	return v;
}
Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	Vector2 v;
	v.X = v1.X - v2.X;
	v.Y = v1.Y - v2.Y;
	return v;
}

Vector2 Vector2::WrongVector{ -1, -1 };