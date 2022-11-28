#pragma once
#include <vector> 
#include <fstream>
#include <string>
#include <thread>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

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
	return params;
}

void loadLayerFromFile(std::string path, TimeLayer& layer) {
	int layerCounter{};
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

	std::vector<std::jthread> threads;

	int step = 0;
	while (true) {
		auto path = std::filesystem::current_path().append(dirPath + "\\data\\" + std::to_string(step));
		if (std::filesystem::exists(path)) {
			grid.emplace_back();
			threads.emplace_back(loadLayerFromFile, path.string(), std::ref(grid.back()));
			step += params.save_step;
		}
		else {
			params.maxTimeStep = step;
			break;
		}
	}

	return tuple;
}