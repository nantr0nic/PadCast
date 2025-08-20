#include "config.h"

void Config::LoadConfig()
{
	configPath = GetConfigFilePath();
	config = mINI::INIFile{ configPath };

	try
	{
		auto configDir{ std::filesystem::path{configPath}.parent_path() };

		if (!std::filesystem::exists(configDir))
		{
			std::println("Creating config directory: {}", configDir.string());
			std::filesystem::create_directories(configDir);
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		std::println("Error: Cannot create config directory - {}", e.what());
		// Fallback: try to use current directory
		configPath = "config.ini";
		std::println("Falling back to current directory");
	}

	if (!config.read(config_ini))
	{
		// config.ini doesn't exist so we'll create one with default values
		std::println("config.ini doesn't exist at {}, creating a default one.", configPath);
		std::filesystem::create_directories(std::filesystem::path{ configPath }.parent_path());
		ValidateConfig();
		SaveConfig();
	}
	ValidateConfig();
}

void Config::ValidateConfig()
{
	bool needsSave{ false };

	// Check Window section
	if (!hasValue("Window", "INITIAL_WINDOW_WIDTH"))
	{
		config_ini["Window"]["INITIAL_WINDOW_WIDTH"] = std::to_string(DefaultValues::INITIAL_WINDOW_WIDTH);
		needsSave = true;
	}
	if (!hasValue("Window", "INITIAL_WINDOW_HEIGHT"))
	{
		config_ini["Window"]["INITIAL_WINDOW_HEIGHT"] = std::to_string(DefaultValues::INITIAL_WINDOW_HEIGHT);
		needsSave = true;
	}
	if (!hasValue("Window", "IMAGE_CANVAS_WIDTH"))
	{
		config_ini["Window"]["IMAGE_CANVAS_WIDTH"] = std::to_string(DefaultValues::IMAGE_CANVAS_WIDTH);
		needsSave = true;
	}
	if (!hasValue("Window", "IMAGE_CANVAS_HEIGHT"))
	{
		config_ini["Window"]["IMAGE_CANVAS_HEIGHT"] = std::to_string(DefaultValues::IMAGE_CANVAS_HEIGHT);
		needsSave = true;
	}
	if (!hasValue("Window", "TARGET_FPS"))
	{
		config_ini["Window"]["TARGET_FPS"] = std::to_string(DefaultValues::TARGET_FPS);
		needsSave = true;
	}
	if (!hasValue("Window", "BACKGROUND_COLOR"))
	{
		config_ini["Window"]["BACKGROUND_COLOR"] = std::to_string(DefaultValues::BACKGROUND_COLOR);
		needsSave = true;
	}

	// Check Gamepad section
	if (!hasValue("Gamepad", "STABILITY_THRESHOLD"))
	{
		config_ini["Gamepad"]["STABILITY_THRESHOLD"] = std::to_string(DefaultValues::STABILITY_THRESHOLD);
		needsSave = true;
	}

	// Check Font section
	if (!hasValue("Font", "MIN_FONT_SIZE"))
	{
		config_ini["Font"]["MIN_FONT_SIZE"] = std::to_string(DefaultValues::MIN_FONT_SIZE);
		needsSave = true;
	}
	if (!hasValue("Font", "DEFAULT_FONT_SIZE"))
	{
		config_ini["Font"]["DEFAULT_FONT_SIZE"] = std::to_string(DefaultValues::DEFAULT_FONT_SIZE);
		needsSave = true;
	}
	if (!hasValue("Font", "TEXT_OFFSET"))
	{
		config_ini["Font"]["TEXT_OFFSET"] = std::to_string(DefaultValues::TEXT_OFFSET);
		needsSave = true;
	}

	// Check Debug section
	if (!hasValue("Debug", "MODE"))
	{
		config_ini["Debug"]["MODE"] = std::to_string(DefaultValues::DEBUG_MODE);
	}

	if (needsSave)
	{
		std::println("Adding missing config values...");
		SaveConfig();
	}
}

int Config::getDefault(const std::string& section, const std::string& key) const
{
	if (section == "Font") 
	{
		if (key == "MIN_FONT_SIZE") 
			return DefaultValues::MIN_FONT_SIZE;
		if (key == "DEFAULT_FONT_SIZE") 
			return DefaultValues::DEFAULT_FONT_SIZE;
		if (key == "TEXT_OFFSET") 
			return DefaultValues::TEXT_OFFSET;
	}
	if (section == "Window") 
	{
		if (key == "INITIAL_WINDOW_WIDTH") 
			return DefaultValues::INITIAL_WINDOW_WIDTH;
		if (key == "INITIAL_WINDOW_HEIGHT") 
			return DefaultValues::INITIAL_WINDOW_HEIGHT;
		if (key == "TARGET_FPS") 
			return DefaultValues::TARGET_FPS;
		if (key == "BACKGROUND_COLOR")
			return DefaultValues::BACKGROUND_COLOR;
	}
	if (section == "Gamepad") 
	{
		if (key == "STABILITY_THRESHOLD") 
			return DefaultValues::STABILITY_THRESHOLD;
	}
	if (section == "Debug") 
	{
		if (key == "MODE") 
			return DefaultValues::DEBUG_MODE;
	}
	// Fallback if unknown
	return 0;
}