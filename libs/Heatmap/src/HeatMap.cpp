#include <fstream>
#include <iostream>
#include <sstream>
#include "HeatMap.h"
#include "SPH2D_FIO.h"

std::unordered_map<std::string, HeatMap::Preset> 
HeatMap::Preset::FindPresets(const std::string& experiment_directory) {
    std::unordered_map<std::string, HeatMap::Preset> presets;
    std::string filename = SPHFIO::instance().getExperimentDirectory()  + "heatmap_presets.txt";
    std::ifstream stream{ filename };

    if (stream.is_open()) {
        std::string line;
        while (std::getline(stream, line)) {
            if (line[0] == '#') continue;

            std::stringstream sstream(line);
            std::string preset_name;
            std::string variable_name;
            double min, max;
            sstream >> preset_name;
            sstream >> variable_name;
            sstream >> min;
            sstream >> max;

            presets[preset_name] = Preset(min, max, variable_name);
        }
    }
    else {
        std::cerr << "can't find presets file " << filename << std::endl;
    }

    return presets;
}