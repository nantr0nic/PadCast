#include "config.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map>

void Config::loadConfig()
{
	mConfigPath = getConfigFilePath();
	mConfigFile = mINI::INIFile{ mConfigPath };

	try
	{
		auto configDir{ std::filesystem::path{mConfigPath}.parent_path() };

		if (!std::filesystem::exists(configDir))
		{
			std::cout << "Creating config directory: " << configDir.string() << std::endl;
			std::filesystem::create_directories(configDir);
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		std::cerr << "Error: Cannot create config directory - " << e.what() << std::endl;
		// Fallback: try to use current directory
		mConfigPath = "config.ini";
		std::cout << "Falling back to current directory" << std::endl;
	}

	if (!mConfigFile.read(config_ini))
	{
		// config.ini doesn't exist so we'll create one with default values
		std::cout << "config.ini doesn't exist at " << mConfigPath << ", creating a default one." << std::endl;
		std::filesystem::create_directories(std::filesystem::path{ mConfigPath }.parent_path());
		validateConfig();
		saveConfig();
	}
	validateConfig();
}

void Config::validateConfig()
{
	mNeedsSave = false;

	//$ ----- Window section ----- //
	validateInt("Window", "INITIAL_WINDOW_WIDTH",  DefaultValues::INITIAL_WINDOW_WIDTH,  [](int val) { return val > 0; });
	validateInt("Window", "INITIAL_WINDOW_HEIGHT", DefaultValues::INITIAL_WINDOW_HEIGHT, [](int val) { return val > 0; });
	validateInt("Window", "TARGET_FPS",            DefaultValues::TARGET_FPS,            [](int val) { return val >= 1 && val <= 250; });
	validateInt("Window", "USE_VSYNC",             DefaultValues::USE_VSYNC,             [](int val) { return val == 0 || val == 1; });
	validateInt("Window", "BACKGROUND_COLOR",      DefaultValues::BACKGROUND_COLOR,      [](int val) { return val >= 0 && val <= 4; });
	validateInt("Window", "CUSTOM_BG_RED",         DefaultValues::CUSTOM_BG_RED,         [](int val) { return val >= 0 && val <= 255; });
	validateInt("Window", "CUSTOM_BG_GREEN",       DefaultValues::CUSTOM_BG_GREEN,       [](int val) { return val >= 0 && val <= 255; });
	validateInt("Window", "CUSTOM_BG_BLUE",        DefaultValues::CUSTOM_BG_BLUE,        [](int val) { return val >= 0 && val <= 255; });
	validateInt("Window", "USE_CUSTOM_BG",         DefaultValues::USE_CUSTOM_BG,         [](int val) { return val == 0 || val == 1; });

	//$ ----- Image section ----- //
	validateInt("Image", "IMAGE_CANVAS_WIDTH",  DefaultValues::IMAGE_CANVAS_WIDTH,  [](int val) { return val > 0; });
	validateInt("Image", "IMAGE_CANVAS_HEIGHT", DefaultValues::IMAGE_CANVAS_HEIGHT, [](int val) { return val > 0; });
	validateInt("Image", "USE_CUSTOM_TINT",     DefaultValues::USE_CUSTOM_TINT,     [](int val) { return val == 0 || val == 1; });
	validateInt("Image", "IMAGE_TINT_RED",      DefaultValues::IMAGE_TINT_RED,      [](int val) { return val >= 0 && val <= 255; });
	validateInt("Image", "IMAGE_TINT_GREEN",    DefaultValues::IMAGE_TINT_GREEN,    [](int val) { return val >= 0 && val <= 255; });
	validateInt("Image", "IMAGE_TINT_BLUE",     DefaultValues::IMAGE_TINT_BLUE,     [](int val) { return val >= 0 && val <= 255; });
	validateInt("Image", "IMAGE_TINT_PALETTE",  DefaultValues::IMAGE_TINT_PALETTE,  [](int val) { return val >= 0 && val <= 3; });

	//$ ----- Gamepad section ----- //
	validateInt("Gamepad", "STABILITY_THRESHOLD", DefaultValues::STABILITY_THRESHOLD, [](int val) { return val > 0; });
	validateInt("Gamepad", "GAMEPAD_INDEX",       DefaultValues::GAMEPAD_INDEX,       [](int val) { return val >= 0 && val <= 3; });

	//$ ----- Font section ----- //
	validateInt("Font", "MIN_FONT_SIZE",     DefaultValues::MIN_FONT_SIZE,     [](int val) { return val > 0; });
	validateInt("Font", "DEFAULT_FONT_SIZE",  DefaultValues::DEFAULT_FONT_SIZE,  [](int val) { return val > 0; });
	validateInt("Font", "TEXT_OFFSET",        DefaultValues::TEXT_OFFSET,        [](int val) { return val >= 0; });

	//$ ----- SNES_ButtonMap section ----- //
	validateInt("SNES_ButtonMap", "DPAD_UP",    SNESMapDefaults::DPAD_UP,    [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "DPAD_RIGHT", SNESMapDefaults::DPAD_RIGHT, [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "DPAD_DOWN",  SNESMapDefaults::DPAD_DOWN,  [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "DPAD_LEFT",  SNESMapDefaults::DPAD_LEFT,  [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "X_BUTTON",   SNESMapDefaults::X_BUTTON,   [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "A_BUTTON",   SNESMapDefaults::A_BUTTON,   [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "B_BUTTON",   SNESMapDefaults::B_BUTTON,   [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "Y_BUTTON",   SNESMapDefaults::Y_BUTTON,   [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "L_BUTTON",   SNESMapDefaults::L_BUTTON,   [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "R_BUTTON",   SNESMapDefaults::R_BUTTON,   [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "SELECT",     SNESMapDefaults::SELECT,     [](int val) { return val > 0; });
	validateInt("SNES_ButtonMap", "START",      SNESMapDefaults::START,      [](int val) { return val > 0; });

	//$ ----- Debug section ----- //
	validateInt("Debug", "MODE", DefaultValues::DEBUG_MODE, [](int val) { return val == 0 || val == 1; });

	if (mNeedsSave)
	{
		std::cout << "Adding missing or invalid config values..." << std::endl;
		saveConfig();
	}
}

int Config::getDefault(const std::string& section, const std::string& key) const
{
	// Composite key is "SectionName:KEY_NAME" — neither sections nor keys contain colons.
	static const std::unordered_map<std::string, int> defaultsLookup{
		{"Window:INITIAL_WINDOW_WIDTH",  DefaultValues::INITIAL_WINDOW_WIDTH},
		{"Window:INITIAL_WINDOW_HEIGHT", DefaultValues::INITIAL_WINDOW_HEIGHT},
		{"Window:TARGET_FPS",            DefaultValues::TARGET_FPS},
		{"Window:USE_VSYNC",             DefaultValues::USE_VSYNC},
		{"Window:BACKGROUND_COLOR",      DefaultValues::BACKGROUND_COLOR},
		{"Window:CUSTOM_BG_RED",         DefaultValues::CUSTOM_BG_RED},
		{"Window:CUSTOM_BG_GREEN",       DefaultValues::CUSTOM_BG_GREEN},
		{"Window:CUSTOM_BG_BLUE",        DefaultValues::CUSTOM_BG_BLUE},
		{"Window:USE_CUSTOM_BG",         DefaultValues::USE_CUSTOM_BG},

		{"Image:IMAGE_CANVAS_WIDTH",  DefaultValues::IMAGE_CANVAS_WIDTH},
		{"Image:IMAGE_CANVAS_HEIGHT", DefaultValues::IMAGE_CANVAS_HEIGHT},
		{"Image:USE_CUSTOM_TINT",     DefaultValues::USE_CUSTOM_TINT},
		{"Image:IMAGE_TINT_RED",      DefaultValues::IMAGE_TINT_RED},
		{"Image:IMAGE_TINT_GREEN",    DefaultValues::IMAGE_TINT_GREEN},
		{"Image:IMAGE_TINT_BLUE",     DefaultValues::IMAGE_TINT_BLUE},
		{"Image:IMAGE_TINT_PALETTE",  DefaultValues::IMAGE_TINT_PALETTE},

		{"Gamepad:STABILITY_THRESHOLD", DefaultValues::STABILITY_THRESHOLD},
		{"Gamepad:GAMEPAD_INDEX",       DefaultValues::GAMEPAD_INDEX},

		{"Font:MIN_FONT_SIZE",     DefaultValues::MIN_FONT_SIZE},
		{"Font:DEFAULT_FONT_SIZE", DefaultValues::DEFAULT_FONT_SIZE},
		{"Font:TEXT_OFFSET",       DefaultValues::TEXT_OFFSET},

		{"SNES_ButtonMap:DPAD_UP",    SNESMapDefaults::DPAD_UP},
		{"SNES_ButtonMap:DPAD_RIGHT", SNESMapDefaults::DPAD_RIGHT},
		{"SNES_ButtonMap:DPAD_DOWN",  SNESMapDefaults::DPAD_DOWN},
		{"SNES_ButtonMap:DPAD_LEFT",  SNESMapDefaults::DPAD_LEFT},
		{"SNES_ButtonMap:X_BUTTON",   SNESMapDefaults::X_BUTTON},
		{"SNES_ButtonMap:A_BUTTON",   SNESMapDefaults::A_BUTTON},
		{"SNES_ButtonMap:B_BUTTON",   SNESMapDefaults::B_BUTTON},
		{"SNES_ButtonMap:Y_BUTTON",   SNESMapDefaults::Y_BUTTON},
		{"SNES_ButtonMap:L_BUTTON",   SNESMapDefaults::L_BUTTON},
		{"SNES_ButtonMap:R_BUTTON",   SNESMapDefaults::R_BUTTON},
		{"SNES_ButtonMap:SELECT",     SNESMapDefaults::SELECT},
		{"SNES_ButtonMap:START",      SNESMapDefaults::START},

		{"Debug:MODE", DefaultValues::DEBUG_MODE},
	};

	std::string compositeKey = section + ":" + key;
	auto lookupIter = defaultsLookup.find(compositeKey);
	if (lookupIter != defaultsLookup.end())
	{
		return lookupIter->second;
	}

	// Fallback if unknown section/key combination
	std::cerr << "Warning: no default for [" << section << "] " << key << std::endl;
	return 0;
}

void Config::resetButtonMap(ControllerLayout layout)
{
	std::string section = buttonSectionName(layout);
	config_ini[section].set({
		{"DPAD_UP",    "1"},
		{"DPAD_RIGHT", "2"},
		{"DPAD_DOWN",  "3"},
		{"DPAD_LEFT",  "4"},
		{"X_BUTTON",   "5"},
		{"A_BUTTON",   "6"},
		{"B_BUTTON",   "7"},
		{"Y_BUTTON",   "8"},
		{"L_BUTTON",   "9"},
		{"R_BUTTON",   "11"},
		{"SELECT",     "13"},
		{"START",      "15"}
	});
	mNeedsSave = true;
	saveConfig();
}

std::unordered_map<std::string, int> Config::loadButtonMapping(ControllerLayout layout) const
{
	std::unordered_map<std::string, int> mapping;
	std::string section = buttonSectionName(layout);

	if (!config_ini.has(section))
	{
		return mapping;  // empty — caller falls back to defaults
	}

	for (const auto& [key, valueStr] : config_ini.get(section))
	{
		try
		{
			mapping[key] = std::stoi(valueStr);
		}
		catch (...)
		{
			// skip invalid entries
		}
	}
	return mapping;
}
