#pragma once
#include "RRColor.h"
#include <map>

class HeatMap {
	double valueRange = 1;
	double min = 0;
	double max = 1;
public:
	void SetNew(double min, double max) {
		valueRange = max - min;
		this->min = min;
		this->max = max;
	}
	RRColor GetNewColorForNum(double value) const {
		constexpr double hueMin = 0;
		constexpr double hueMax = 255. / 360.;
		constexpr double hueRange = hueMax - hueMin;
		constexpr double sat = 240. / 255.;
		constexpr double lum = 113. / 255.;

		auto normValue = (value - min) / valueRange;
		auto hue = normValue * hueRange + hueMin;
		return RRColor::FromHSL(hueMax - hue, sat, lum);
	}

	std::map<double, RRColor, std::greater<double>> GetLegend(int values = 4) const {
		double step = valueRange / values;
		std::map<double, RRColor, std::greater<double>> legend;

		for (int i = values; i >= 0; i--) {
			auto value = max - i * step;
			legend.emplace(value, GetNewColorForNum(value));
		}
		return legend;
	}
};