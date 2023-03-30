#pragma once
#include <vector>
#include <string>

struct ExperimentParams {
	std::string experiment_name;
	std::string format_line;
	double x_origin;
	double y_origin;
	double x_size;
	double y_size;
	double dx;
	double dy;
	double dt;
	double hsml;
	double simulation_time;
	int save_step;
	double depth;
	double wave_length;
	int skf;
	int particles_fluid;
	int particles_boundary;
	int particles_total;
};

struct Particle {
	double x;
	double y;
	int itype;

	double vx;
	double vy;
	double p;
	double rho;
	double u;

	double byTag(const std::string& tag) const;
};
using TimeLayer = std::vector<Particle>;
using Grid = std::vector<TimeLayer>;
using Square = std::pair<std::pair<double, double>, std::pair<double, double>>;

class SPHFIO {
public:
	static SPHFIO& instance() {
		static SPHFIO sphfio;
		return sphfio;
	}
	void init(std::string experiment_name);

	const ExperimentParams& getParams() const {
		return params;
	}
	const Square& getSquare() const {
		return square;
	}
	const Grid& getGrid() const {
		return grid;
	}
	Grid takeGrid() {
		return std::move(grid);
	}

	const std::string& getExperimentName() const {
		return experiment_name;
	}
	const std::string& getExperimentDirectory() const {
		return experiment_directory;
	}
	const std::string& getAnalysisDirectory() const {
		return analysis_directory;
	}
	const std::string& getScreenshotsDirectory() const {
		return screenshots_directory;
	}
	const std::string& getVideosRawDirectory() const {
		return videos_raw_directory;
	}
	const std::string& getVideosDirectory() const {
		return videos_directory;
	}

	bool isAdditionalValuePresented(const char* value) const;

private:
	SPHFIO() = default;
	SPHFIO(SPHFIO&) = delete;
	auto operator=(SPHFIO&) = delete;

	static ExperimentParams loadParams(std::string_view filePath);
	static std::vector<std::string> findTimeLayersPath(const ExperimentParams& params);
	static std::vector<const char*> findAdditionalValues(const ExperimentParams& params);
	void initDrawingFilesystem();
	static void loadLayerFromFileMM(std::string_view filename, TimeLayer& layer, const std::vector<const char*>& additional_values_index);
	static Square loadSquare(const ExperimentParams& params);
	static Grid loadGrid(const ExperimentParams& params, const std::vector<const char*>& additional_values_index);
private:
	std::string experiment_name;
	std::string experiment_directory;
	std::string analysis_directory;
	std::string screenshots_directory;
	std::string videos_raw_directory;
	std::string videos_directory;

	ExperimentParams params;
	std::vector<const char*> additional_values_index;
	Square square;
	Grid grid;
};