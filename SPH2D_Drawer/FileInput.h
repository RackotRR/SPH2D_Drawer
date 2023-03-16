#pragma once
#include <vector> 
#include <fstream>
#include <string>
#include <thread>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <mio/mio.hpp>
#include "RRGrapher.h"


struct Params {
	std::string experimentName;
	double x_origin;
	double y_origin;
	double x_size;
	double y_size;
	double dx;
	double dy;
	double dt;
	double simulationTime;
	int save_step;
	int maxTimeStep;
	double depth;
	double wave_length;
};


inline Params loadParams(std::string filePath) {
	nlohmann::json json;
	std::ifstream stream{ filePath };
	stream >> json;

	Params params;
	json.at("experiment_name").get_to(params.experimentName);
	json.at("x_mingeom").get_to(params.x_origin);
	json.at("y_mingeom").get_to(params.y_origin);
	json.at("x_size").get_to(params.x_size);
	json.at("y_size").get_to(params.y_size);
	json.at("dx").get_to(params.dx);
	json.at("dy").get_to(params.dy);
	json.at("dt").get_to(params.dt);
	json.at("simulation_time").get_to(params.simulationTime);
	json.at("save_step").get_to(params.save_step);
	json.at("depth").get_to(params.depth);
	json.at("wave_length").get_to(params.wave_length);
	return params;
}

namespace SPHFIO {
	inline std::string EXPERIMENT_NAME;
	inline std::string EXPERIMENT_DIRECTORY;
	inline std::string SCREENSHOTS_PATH;
	inline std::string VIDEOS_RAW_PATH;
	inline std::string VIDEOS_PATH;

	inline void initDrawingFilesystem(std::string experiment_name) {
		EXPERIMENT_NAME = experiment_name;
		EXPERIMENT_DIRECTORY = EXPERIMENT_NAME + "\\";

		auto screenshots_path = std::filesystem::current_path().append(SPHFIO::EXPERIMENT_DIRECTORY + "screenshots\\");
		std::filesystem::create_directory(screenshots_path);
		SCREENSHOTS_PATH = screenshots_path.string();

		auto videos_path = std::filesystem::current_path().append(SPHFIO::EXPERIMENT_DIRECTORY + "videos\\");
		auto videos_raw_path = std::filesystem::current_path().append(SPHFIO::EXPERIMENT_DIRECTORY + "videos\\raw\\");
		std::filesystem::create_directory(videos_path);
		std::filesystem::create_directory(videos_raw_path);
		VIDEOS_PATH = videos_path.string();
		VIDEOS_RAW_PATH = videos_raw_path.string();
	}

	constexpr char VX_NAME[] = " vx ";
	constexpr char VY_NAME[] = " vy ";
	constexpr char P_NAME[] = " p ";
	constexpr char U_NAME[] = " u ";
	constexpr char RHO_NAME[] = " rho ";

	inline std::map<size_t, const char*> getAvailableValues(std::string_view format_line) {
		std::map<size_t, const char*> available_values;
		auto check_value_availability = [&](const char* value) {
			size_t pos = format_line.find(value);
			if (pos != format_line.npos) {
				available_values[pos] = value;
			}
		};

		check_value_availability(VX_NAME);
		check_value_availability(VY_NAME);
		check_value_availability(P_NAME);
		check_value_availability(U_NAME);
		check_value_availability(RHO_NAME);

		return available_values;
	}

	inline void loadLayerFromFileMM(std::string filename, TimeLayer& layer) {
		std::error_code error;
		auto mmap = mio::make_mmap_source(filename.c_str(), error);
		if (error) {
			std::cout << "error mapping file " << error.message() << ", exit" << std::endl;
			return;
		}

		// format
		const char fmt_string[] = "fmt: ";
		std::string_view str(mmap.begin(), mmap.end());
		size_t format_line_size = str.find('\n');
		if (format_line_size == str.npos) {
			throw std::runtime_error{ "file" + filename + " was empty" };
		}
		// additional values
		std::map<const char*, double> additional_values;
		std::vector<const char*> additional_values_index;
		if (str.starts_with(fmt_string)) {
			std::string_view format_line(mmap.begin(), format_line_size);
			auto available_values = getAvailableValues(format_line);

			for (auto& [pos, value] : available_values) {
				additional_values_index.push_back(value);
			}
		}
		else {
			format_line_size = 0;
		}

		auto begin = std::begin(mmap) + format_line_size;
		char* iter;

		size_t ntotal = std::strtoull(begin, &iter, 10);
		layer.reserve(ntotal);

	double press;
	double v_x, v_y;

		double x, y;
		long type;
		for (; iter != std::end(mmap) && layer.size() < ntotal;) {
			x = std::strtod(iter, &iter);
			y = std::strtod(iter, &iter);
			type = std::strtol(iter, &iter, 10);
		v_x = std::strtod(iter, &iter);
		v_y = std::strtod(iter, &iter);
		press = std::strtod(iter, &iter);
		layer.emplace_back(x, y, type);
	}

			for (const char* value_name : additional_values_index) {
				double value = std::strtod(iter, &iter);
				additional_values[value_name] = value;
			}

			layer.emplace_back(
				x, y, type,
				additional_values[VX_NAME],
				additional_values[VY_NAME],
				additional_values[P_NAME],
				additional_values[RHO_NAME],
				additional_values[U_NAME]);
		}

		std::filesystem::path path = filename;
		std::string output = "layer " + path.filename().string() + "\n";
		std::cout << output;
	}
}

inline auto ReadGridAndParams() {
	std::tuple<Grid, Square, double> tuple;
	auto& [grid, square, dx] = tuple;
	auto& [origin, size] = square;

	Params params = loadParams(SPHFIO::EXPERIMENT_DIRECTORY + "Params.json");
	dx = params.dx;
	auto& [origin_x, origin_y] = origin;
	auto& [size_x, size_y] = size;
	origin_x = params.x_origin;
	origin_y = params.y_origin;
	size_x = params.x_size;
	size_y = params.y_size;

	std::vector<std::string> meta;

	int step = 0;
	while (true) {
		auto path = std::filesystem::current_path().append(SPHFIO::EXPERIMENT_DIRECTORY + "data\\" + std::to_string(step));
		if (std::filesystem::exists(path)) {
			meta.emplace_back(path.string());
			step += params.save_step;
		}
		else {
			params.maxTimeStep = step;
			break;
		}
	}

	grid = std::vector<TimeLayer>(meta.size());

#pragma omp parallel for
	for (int i = 0; i < meta.size(); ++i) {
		auto& path = meta[i];
		SPHFIO::loadLayerFromFileMM(path, grid[i]);
	}

	std::cout << "finish loading" << std::endl;
	return tuple;
}
