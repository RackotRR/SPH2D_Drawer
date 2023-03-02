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


Params loadParams(std::string filePath) {
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

void loadLayerFromFileMM(std::string path, TimeLayer& layer) {
	std::error_code error;
	auto mmap = mio::make_mmap_source(path.c_str(), error);
	if (error) {
		std::cout << "error mapping file " << error.message() << ", exit" << std::endl;
		return;
	}

	auto begin = std::begin(mmap);
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

	std::filesystem::path p = path;
	std::string output = "layer " + p.filename().string() + "\n";
	std::cout << output;
}

void loadLayerFromFile(std::string path, TimeLayer& layer) {
	std::ifstream stream(path);

	size_t ntotal;
	stream >> ntotal;

	layer.reserve(ntotal);

	for (size_t i{}; i < ntotal; i++) {
		double x, y;
		int type;
		stream >> x >> y;
		stream >> type;
		layer.emplace_back(x, y, type);
	}

	std::filesystem::path p = path;
	std::string output = "layer " + p.filename().string() + "\n";
	std::cout << output;
}

auto ReadGridAndParams(std::string dirPath) {
	std::tuple<Grid, Square, double> tuple;
	auto& [grid, square, dx] = tuple;
	auto& [origin, size] = square;

	Params params = loadParams(dirPath + "\\Params.json");
	dx = params.dx;
	origin.first = params.x_origin;
	origin.second = params.y_origin;
	size.first = params.x_size;
	size.second = params.y_size;

	std::vector<std::string> meta;

	int step = 0;
	while (true) {
		auto path = std::filesystem::current_path().append(dirPath + "\\data\\" + std::to_string(step));
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
		loadLayerFromFileMM(path, grid[i]);
	}

	std::cout << "finish loading" << std::endl;
	return tuple;
}
