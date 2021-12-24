#pragma once
/*
Абстракция цвета
*/
struct RRColor {
	constexpr RRColor() : RRColor{ 0, 0, 0, 255 } {}
	constexpr RRColor(unsigned char r, unsigned char g, unsigned char b) : RRColor{ r, g, b, 255 } {}
	constexpr RRColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r{ r }, g{ g }, b{ b }, a{ a } {}
	
	unsigned char r, g, b, a; 

	static consteval RRColor White() {
		return RRColor{ 255, 255, 255 };
	}  
	static consteval RRColor Black() {
		return RRColor{};
	}
	static consteval RRColor Red() {
		return RRColor{ 255, 0, 0 };
	}
	static consteval RRColor Green() {
		return RRColor{ 0, 255, 0 };
	}
	static consteval RRColor Blue() {
		return RRColor{ 0, 0, 255 };
	}
}; 