#pragma once
#include <functional>

/*
Вектор или точка, контейнер для координаты
*/
struct Vector2 { 
	Vector2(int x, int y) : X{ x }, Y{ y } {}
	Vector2() : X{}, Y{} {}

	friend bool operator==(const Vector2& v1, const Vector2& v2);
	friend bool operator!=(const Vector2& v1, const Vector2& v2);
	friend Vector2 operator+(const Vector2& v1, const Vector2& v2);
	friend Vector2 operator-(const Vector2& v1, const Vector2& v2);

	Vector2& operator+=(const Vector2& v) {
		*this = *this + v;
		return *this;
	}
	Vector2& operator-=(const Vector2& v) {
		*this = *this - v;
		return *this;
	}
	Vector2& operator*=(int scalar) {
		X *= scalar;
		Y *= scalar;
		return *this;
	}
	Vector2& operator/=(int scalar) {
		X /= scalar;
		Y /= scalar;
		return *this;
	}

	int X, Y;
	static Vector2 WrongVector;
};

// Хэш функтор для unordered map, т.к. Vector2 - ключ
class HashVector2 {
public:
	size_t operator() (const Vector2& v) const {
		std::hash<int> h;
		return (h(v.X) ^ h(v.Y * 1234));
	}
};

