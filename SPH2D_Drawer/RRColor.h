#pragma once
#include <cmath>

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

	static RRColor FromHSL(double h, double s, double l) {
		double r, g, b;

		if (s == 0) {
			r = g = b = l; // achromatic
		}
		else {
			auto hue2rbg = [](double p, double q, double t) {
				if (t < 0) t += 1;
				if (t > 1) t -= 1;
				if (t < 1. / 6.) return p + (q - p) * 6 * t;
				if (t < 0.5) return q;
				if (t < 2. / 3.) return p + (q - p) * (2. / 3. - t) * 6;
				return p;
			};

			auto q = l < 0.5 ? l * (1 + s) : l + s - l * s;
			auto p = 2 * l - q;
			r = hue2rbg(p, q, h + 1. / 3.);
			g = hue2rbg(p, q, h);
			b = hue2rbg(p, q, h - 1. / 3.);
		}

		return RRColor(round(r * 255), round(g * 255), round(b * 255));
	}

	bool operator<(const RRColor& c) const {
		return (r + g * 255 + b * 255 * 255) < (c.r + c.g * 255 + c.b * 255 * 255);
	}
}; 