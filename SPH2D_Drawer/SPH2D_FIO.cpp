#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <mio/mio.hpp>
#include <fstream>
#include <unordered_map>
#include <map>

#include "SPH2D_FIO.h"

static constexpr char VX_NAME[] = " vx ";
static constexpr char VY_NAME[] = " vy ";
static constexpr char P_NAME[] = " p ";
static constexpr char U_NAME[] = " u ";
static constexpr char RHO_NAME[] = " rho ";

double Particle::byTag(const std::string& tag) const {
	static const std::unordered_map<std::string, const double*> dict{
		{ VX_NAME, &vx },
		{ VY_NAME, &vy },
		{ P_NAME, &p },
		{ U_NAME, &u },
		{ RHO_NAME, &rho }
	};

	return *dict.at(tag);
}

static std::map<size_t, const char*> getAvailableValues(const std::string& format_line) {
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

std::vector<const char*> SPHFIO::findAdditionalValues(const ExperimentParams& params) {
	std::vector<const char*> additional_values_index;
	auto available_values = getAvailableValues(params.format_line);

	for (auto& [pos, value] : available_values) {
		additional_values_index.push_back(value);
	}

	return additional_values_index;
}

ExperimentParams SPHFIO::loadParams(std::string_view filePath) {
	nlohmann::json json;
	std::ifstream stream{ filePath.data() };
	stream >> json;

	ExperimentParams params;
	if (json.contains("experiment_name")) {
		json.at("experiment_name").get_to(params.experiment_name);
	}
	else {
		json.at("experimentName").get_to(params.experiment_name);
	}
	
	if (json.contains("format_line")) {
		json.at("format_line").get_to(params.format_line);
	}
	else {
		params.format_line = " vx vy p ";
	}

	json.at("x_mingeom").get_to(params.x_origin);
	json.at("y_mingeom").get_to(params.y_origin);
	double x_maxgeom; json.at("x_maxgeom").get_to(x_maxgeom); params.x_size = x_maxgeom - params.x_origin;
	double y_maxgeom; json.at("y_maxgeom").get_to(y_maxgeom); params.y_size = y_maxgeom - params.y_origin;
	double delta;
	json.at("delta").get_to(delta); params.dx = params.dy = delta;
	json.at("dt").get_to(params.dt);
	json.at("hsml").get_to(params.hsml);
	json.at("simulation_time").get_to(params.simulation_time);
	json.at("save_step").get_to(params.save_step);
	json.at("skf").get_to(params.skf);
	json.at("nfluid").get_to(params.particles_fluid);
	json.at("nvirt").get_to(params.particles_boundary);
	json.at("ntotal").get_to(params.particles_total);
	return params;
}

void SPHFIO::init(std::string experiment_name) {
	this->experiment_name = experiment_name;
	this->experiment_directory = experiment_name + '\\';
	this->analysis_directory = experiment_directory + "analysis\\";
	this->params = loadParams(experiment_directory + "Params.json");
	this->additional_values_index = findAdditionalValues(params);
	this->square = loadSquare(params);
	this->grid = loadGrid(params, additional_values_index);
	initDrawingFilesystem();
}

void SPHFIO::initDrawingFilesystem() {
	auto screenshots_path = std::filesystem::current_path().append(experiment_directory + "screenshots\\");
	std::filesystem::create_directory(screenshots_path);
	this->screenshots_directory = screenshots_path.string();

	auto videos_path = std::filesystem::current_path().append(experiment_directory + "videos\\");
	auto videos_raw_path = std::filesystem::current_path().append(experiment_directory + "videos\\raw\\");
	std::filesystem::create_directory(videos_path);
	std::filesystem::create_directory(videos_raw_path);
	this->videos_directory = videos_path.string();
	this->videos_raw_directory = videos_raw_path.string();
}



void SPHFIO::loadLayerFromFileMM(std::string_view filename, TimeLayer& layer, const std::vector<const char*>& additional_values_index) {
	std::error_code error;
	auto mmap = mio::make_mmap_source(filename.data(), error);
	if (error) {
		throw std::runtime_error("error mapping file " + error.message() + ", exit");
	}

	// format
	const char fmt_string[] = "fmt: ";
	std::string_view str(mmap.begin(), mmap.end());
	size_t format_line_size = str.find('\n');
	if (format_line_size == str.npos) {
		throw std::runtime_error{ "file" + std::string{ filename } + " was empty" };
	}

	// additional values
	std::unordered_map<const char*, double> additional_values;
	if (!str.starts_with(fmt_string)) {
		format_line_size = 0;
	}

	auto begin = std::begin(mmap) + format_line_size;
	char* iter;

	size_t ntotal = std::strtoull(begin, &iter, 10);
	layer.reserve(ntotal);

	double x, y;
	long type;
	for (; iter != std::end(mmap) && layer.size() < ntotal;) {
		x = std::strtod(iter, &iter);
		y = std::strtod(iter, &iter);
		type = std::strtol(iter, &iter, 10);

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

Square SPHFIO::loadSquare(const ExperimentParams& params) {
	Square square;
	auto& [origin, size] = square;
	auto& [origin_x, origin_y] = origin;
	auto& [size_x, size_y] = size;
	origin_x = params.x_origin;
	origin_y = params.y_origin;
	size_x = params.x_size;
	size_y = params.y_size;
	return square;
}

std::vector<std::string> SPHFIO::findTimeLayersPath(const ExperimentParams& params) {
	std::vector<std::string> meta;
	int step = 0;
	while (true) {
		auto path = std::filesystem::current_path().append(params.experiment_name + "\\data\\" + std::to_string(step));
		if (std::filesystem::exists(path)) {
			meta.emplace_back(path.string());
			step += params.save_step;
		}
		else {
			break;
		}
	}
	return meta;
}

Grid SPHFIO::loadGrid(const ExperimentParams& params, const std::vector<const char*>& additional_values_index) {
	auto time_layers_path = findTimeLayersPath(params);
	Grid grid = std::vector<TimeLayer>(time_layers_path.size());

#pragma omp parallel for
	for (int i = 0; i < grid.size(); ++i) {
		SPHFIO::loadLayerFromFileMM(time_layers_path[i], grid[i], additional_values_index);
	}

	std::cout << "finish loading" << std::endl;
	return grid;
}

bool SPHFIO::isAdditionalValuePresented(const char* value) const {
	for (const char* cmp_value : additional_values_index) {
		if (strcmp(value, cmp_value) == 0) {
			return true;
		}
	}
	return false;
}