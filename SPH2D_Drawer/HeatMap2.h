#pragma once
#include "RRColor.h"
#include <map>
#include <string>
#include <unordered_map>

class HeatMap {
public:
	class Preset {
	public:
		Preset() : Preset(0, 1, "") {}
		Preset(double min, double max, std::string variableName) : 
			min_{ min }, 
			max_{ max }, 
			valueRange_{ max - min },
			variableName_{ variableName }
		{
		}

		inline const double& Min() const {
			return min_;
		}
		inline const double& Max() const {
			return max_;
		}
		inline const double& ValueRange() const {
			return valueRange_;
		}
		inline const std::string& VariableName() const {
			return variableName_;
		}

		static std::unordered_map<std::string, Preset> FindPresets();
	private:
		double min_;
		double max_;
		double valueRange_;
		std::string variableName_;
	};
public:
	void SetNew(Preset preset) {
		this->preset = preset;
	}
	void SetNew(double min, double max, std::string variableName) {
		preset = Preset(min, max, std::move(variableName));
	}
	RRColor GetNewColorForNum(double value) const {
		constexpr double hueMin = 0;
		constexpr double hueMax = 255. / 360.;
		constexpr double hueRange = hueMax - hueMin;
		constexpr double sat = 240. / 255.;
		constexpr double lum = 113. / 255.;

		auto normValue = (value - preset.Min()) / preset.ValueRange();
		auto hue = normValue * hueRange + hueMin;
		return RRColor::FromHSL(hueMax - hue, sat, lum);
	}

	std::map<double, RRColor, std::greater<double>> GetLegend(int values = 4) const {
		double step = preset.ValueRange() / values;
		std::map<double, RRColor, std::greater<double>> legend;

		for (int i = values; i >= 0; i--) {
			auto value = preset.Max() - i * step;
			legend.emplace(value, GetNewColorForNum(value));
		}
		return legend;
	}
 
	const std::string& GetVariableName() const {
		return preset.VariableName();
	}
private:
	Preset preset;
};