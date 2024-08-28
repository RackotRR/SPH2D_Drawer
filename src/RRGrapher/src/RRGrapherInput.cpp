#include "RRGrapher.h"
#include "RRController.h"
#include "RRGameIO.h"

#include "SPH2D_FIO.h"
#include <fmt/format.h>
#include <filesystem>
#include <fstream>

void RRGrapher::UpdateControls() {
	auto& controller{ RRController::Instance() };

	auto& mouseState{ controller.GetMouseState() };
	if (mouseState.RightButton == Mouse::State::Pressed) {
		Vector2 deltaPos{ mouseState.Position - mouseState.PrevPosition };
		deltaX += deltaPos.X;
		deltaY -= deltaPos.Y;
	}

	auto& keyState{ controller.GetKeyState() };
	if (keyState.Click(RRKeyboardState::Keys::Q)) {
		scaleCoord *= 1.1;
	}
	if (keyState.Click(RRKeyboardState::Keys::E)) {
		scaleCoord *= 0.9;
	}

	if (keyState.Click(RRKeyboardState::Keys::ENTER)) {
		autoPlay = false;
		currentLayer = 0;
	}
	if (keyState.Click(RRKeyboardState::Keys::SPACE)) {
		autoPlay = !autoPlay;
		if (currentLayer == grid->size() - 1) {
			currentLayer = 0;
		}
	}

	if (keyState.Click(RRKeyboardState::Keys::D) || keyState.IsKeyDown(RRKeyboardState::Keys::W)) {
		if (currentLayer < grid->size() - 1) {
			currentLayer++;
		}
		std::cout << "currentLayer: " << currentLayer << " / " << grid->size() << std::endl;
	}
	if (keyState.Click(RRKeyboardState::Keys::A) || keyState.IsKeyDown(RRKeyboardState::Keys::S)) {
		if (currentLayer > 0) {
			currentLayer--;
		}
		std::cout << "currentT: " << currentLayer << " / " << grid->size() << std::endl;
	}

	if (keyState.IsKeyDown(RRKeyboardState::Keys::Z)) {
		timeToLayer -= 2;
		if (timeToLayer < 2) timeToLayer = 1;
		std::cout << "timeToLayer: " << timeToLayer << std::endl;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::X)) {
		timeToLayer += 2;
		std::cout << "timeToLayer: " << timeToLayer << std::endl;
	}

	if (keyState.IsKeyDown(RRKeyboardState::Keys::I)) {
		deltaY -= spaceSpeed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::K)) {
		deltaY += spaceSpeed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::J)) {
		deltaX += spaceSpeed;
	}
	if (keyState.IsKeyDown(RRKeyboardState::Keys::L)) {
		deltaX -= spaceSpeed;
	}

	if (keyState.Click(RRKeyboardState::Keys::C)) {
		std::string filename = fmt::format("{}_{}_{}", heatMap.GetVariableName(), currentLayer, time(NULL));
		RRGameIO::Instance().MakeScreenshot(sphfio->directories.getScreenshotsDirectory().string() + filename);
	}

	if (keyState.IsKeyDown(RRKeyboardState::Keys::V)) {
		if (lastRenderedLayer != currentLayer) {
			lastRenderedLayer = currentLayer;
			std::string video_name = fmt::format("{}_{}", heatMap.GetVariableName(), videoCounter);
			std::string filename = fmt::format("{}_{}", video_name, renderFrameCounter++);
			std::string directory = sphfio->directories.getVideosRawDirectory().string() + video_name;
			std::filesystem::create_directory(directory);
			RRGameIO::Instance().MakeScreenshot(fmt::format("{0}/{1}", directory, filename));
		}
	}
	else {
		renderFrameCounter = 0;
		lastRenderedLayer = ULLONG_MAX;
		if (keyState.OldIsKeyDown(RRKeyboardState::Keys::V)) {
			std::string video_name = fmt::format("{}_video_{}_{}", 
				sphfio->directories.getExperimentName().string(), 
				heatMap.GetVariableName(), 
				videoCounter);
			std::string directory = fmt::format("{}{}_{}", 
				sphfio->directories.getVideosRawDirectory().string(), 
				heatMap.GetVariableName(), 
				videoCounter);
			std::string command = fmt::format("ffmpeg -framerate 30 -i {}/{}_{}_%%d.png -c:v libx264 -pix_fmt yuv420p {}.mp4", 
				directory, 
				heatMap.GetVariableName(), 
				videoCounter, 
				video_name);

			std::ofstream stream(
				fmt::format("{}{}.bat", sphfio->directories.getVideosDirectory().string(), video_name)
				);
			stream << "@echo off" << std::endl << command << std::endl;
			videoCounter++;
		}
	}

	if (keyState.Click(RRKeyboardState::Keys::F1)) {
		UpdateConsoleInput();
	}
}

void RRGrapher::UpdateConsoleInput() {
	std::string command;
	std::cout << "print \"exit\" to finish console input" << std::endl;
	autoPlay = false;

	while (std::cout << ">> ",
		std::cin >> command,
		command != "exit")
	{
		if (commands.contains(command)) {
			auto& response = commands[command];
			response();
			UpdateDraw();
		}
		else {
			std::cout << "unqnown command" << std::endl;
		}
	}
}
void RRGrapher::InitConsoleCommands() {
	commands.emplace("set", [&] {
		std::string var_name;
		std::cin >> var_name;

		if (var_name == "help") {
			std::cout << "set variable_name value" << std::endl;
#define POSSIBLE_VARIABLE_TO_SET(variable) std::cout << typeid(variable).name() << " " << #variable << " = " << variable << std::endl
			std::cout << "possible variables to set:" << std::endl;
			POSSIBLE_VARIABLE_TO_SET(timeToLayer);
			POSSIBLE_VARIABLE_TO_SET(scaleCoord);
			POSSIBLE_VARIABLE_TO_SET(spaceSpeed);
			POSSIBLE_VARIABLE_TO_SET(deltaX);
			POSSIBLE_VARIABLE_TO_SET(deltaY);
			POSSIBLE_VARIABLE_TO_SET(particleSize);
			POSSIBLE_VARIABLE_TO_SET(currentLayer);
			POSSIBLE_VARIABLE_TO_SET(scientificMode);
			POSSIBLE_VARIABLE_TO_SET(enableCoords);
			POSSIBLE_VARIABLE_TO_SET(certainTypes);
			POSSIBLE_VARIABLE_TO_SET(showType);
#undef POSSIBLE_VARIABLE_TO_SET
		}

#define COMMAND_SET_VARIABLE(variable) if (var_name == #variable) { std::cin >> variable; return; }
		COMMAND_SET_VARIABLE(timeToLayer);
		COMMAND_SET_VARIABLE(scaleCoord);
		COMMAND_SET_VARIABLE(spaceSpeed);
		COMMAND_SET_VARIABLE(deltaX);
		COMMAND_SET_VARIABLE(deltaY);
		COMMAND_SET_VARIABLE(particleSize);
		COMMAND_SET_VARIABLE(currentLayer);
		COMMAND_SET_VARIABLE(scientificMode);
		COMMAND_SET_VARIABLE(enableCoords);
		COMMAND_SET_VARIABLE(certainTypes);
		COMMAND_SET_VARIABLE(showType);
#undef COMMAND_SET_VARIABLE
	});

	commands.emplace("default", [&] {
		DefaultSetup();
	});

	commands.emplace("heatmap", [&] {
		std::string subcommand;
		std::cin >> subcommand;
		if (subcommand == "help") {
			std::cout << "heatmap set variable_name min max" << std::endl;
			std::cout << "heatmap preset install preset_name" << std::endl;
		}
		else if (subcommand == "set") {
			std::string variable_name;
			double min, max;
			std::cout << "variable name: ";
			std::cin >> variable_name;
			std::cout << "min: ";
			std::cin >> min;
			std::cout << "max: ";
			std::cin >> max;
			HeatMap::Preset preset{ min, max, variable_name };
			heatMap.SetNew(preset);
			std::cout << "preset installed" << std::endl;
		}
		else if (subcommand == "preset") {
			auto heatmap_presets = HeatMap::Preset::FindPresets(sphfio->directories.getExperimentDirectory().string());
			for (auto& [preset_name, preset] : heatmap_presets) {
				std::cout << preset_name << ": " << preset.VariableName() << " (" << preset.Min() << "; " << preset.Max() << ")" << std::endl;
			}
			
			if (heatmap_presets.empty()) {
				return;
			}

			std::string subsubcommand;
			while (std::cout << "exit | install [preset_name]:" << std::endl,
				std::cin >> subsubcommand,
				subsubcommand != "exit")
			{
				if (subsubcommand == "install") {
					std::string preset_name;
					while (std::cout << "preset name: ",
						std::cin >> preset_name,
						preset_name != "exit")
					{
						if (heatmap_presets.contains(preset_name)) {
							heatMap.SetNew(heatmap_presets[preset_name]);
							return;
						}
					}
				}
				else {
					std::cout << "unqnown command" << std::endl;
				}
			}
		}
	});

	commands.emplace("help", [&] {
		std::cout << "input:" << std::endl;
		std::cout << "escape: close application" << std::endl;
		std::cout << "F1: console input" << std::endl;
		std::cout << "q / e: scaling" << std::endl;
		std::cout << "a / d: move by one time layer" << std::endl;
		std::cout << "w / s: move time layers by holding" << std::endl;
		std::cout << "ijkl: space navigation " << std::endl;
		std::cout << "space: play / stop animation" << std::endl;
		std::cout << "enter: return at default position" << std::endl;
		std::cout << "z / x: change animation speed" << std::endl;
		std::cout << "commands: " << std::endl;
		for (auto& [command, func] : commands) {
			std::cout << command << std::endl;
		}
	});
}