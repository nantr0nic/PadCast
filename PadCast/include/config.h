#ifndef PADCAST_CONFIG_H
#define PADCAST_CONFIG_H

#include "mini/ini.h"
#include <filesystem>
#include <string>
#include <print>

class Config
{
private:
	std::string configPath{};
	mINI::INIFile config{ configPath };
	mINI::INIStructure config_ini;

	struct DefaultValues
	{
		// Window defaults
		static constexpr int INITIAL_WINDOW_WIDTH{ 960 };
		static constexpr int INITIAL_WINDOW_HEIGHT{ 540 };
		static constexpr int TARGET_FPS{ 60 };
		// Image canvas dimensions
		static constexpr int IMAGE_CANVAS_WIDTH{ 1280 };
		static constexpr int IMAGE_CANVAS_HEIGHT{ 720 };
		// Controller defaults
		static constexpr int STABILITY_THRESHOLD{ 5 };
		// Font defaults
		static constexpr int MIN_FONT_SIZE{ 10 };
		static constexpr int DEFAULT_FONT_SIZE{ 35 };
		static constexpr int TEXT_OFFSET{ 50 };
		// Debug defaults
		static constexpr int DEBUG_MODE{ 0 };
	};

public:
	Config() { LoadConfig(); }
	~Config() { SaveConfig(); }

	//$ ----- config.ini functions (load, save, etc.) ----- //
	// this assumes that the executable is in the root directory
	std::string GetConfigFilePath() const
	{
		return (std::filesystem::current_path() / "config" / "config.ini").string();
	}

	void LoadConfig();

	void ValidateConfig();

	bool hasValue(const std::string& section, const std::string& key) const
	{
		if (!config_ini.has(section))
		{
			return false;
		}
		auto sectionData{ config_ini.get(section) };
		return sectionData.has(key);
	}

	bool SaveConfig()
	{
		return config.write(config_ini);
	}

	//$ ----- getters ----- //
	// so far all the values are ints, so we'll just keep this function
	// but it would be fun to write a template at a later time :)
	int getValue(const std::string& section, const std::string& key) const
	{
		if (!hasValue(section, key))
		{
			std::println("{} or {} doesn't exist, setting with default value.", section, key);
			return getDefault(section, key);
		}
		try
		{
			return std::stoi(config_ini.get(section).get(key));
		}
		catch (const std::invalid_argument&)
		{
			std::println("Wrong value type {} for {}, using default", key, section);
			return getDefault(section, key);
		}
	}

	int getInitWinWidth() const
	{
		return getValue("Window", "INITIAL_WINDOW_WIDTH");
	}
	int getInitWinHeight() const
	{
		return getValue("Window", "INITIAL_WINDOW_HEIGHT");
	}
	int getImgCanvasWidth() const
	{
		return getValue("Window", "IMAGE_CANVAS_WIDTH");
	}
	int getImgCanvasHeight() const
	{
		return getValue("Window", "IMAGE_CANVAS_HEIGHT");
	}
	int getCurrentWinWidth() const
	{
		return getValue("Window", "CURRENT_WINDOW_WIDTH");
	}
	int getCurrentWinHeight() const
	{
		return getValue("Window", "CURRENT_WINDOW_HEIGHT");
	}
	int getFPS() const
	{
		return getValue("Window", "TARGET_FPS");
	}
	int getDebugMode() const
	{
		return getValue("Debug", "MODE");
	}
	int getDefault(const std::string& section, const std::string& key) const;

//$ ----- Setters -----
	void updateWindowSize(int width, int height)
	{
		config_ini["Window"]["CURRENT_WINDOW_WIDTH"] = std::to_string(width);
		config_ini["Window"]["CURRENT_WINDOW_HEIGHT"] = std::to_string(height);
	}
	void updateInitWinSizes()
	{
		config_ini["Window"]["INITIAL_WINDOW_WIDTH"] = config_ini["Window"]["CURRENT_WINDOW_WIDTH"];
		config_ini["Window"]["INITIAL_WINDOW_HEIGHT"] = config_ini["Window"]["CURRENT_WINDOW_HEIGHT"];
	}
	void updateTargetFPS(int fps)
	{
		config_ini["Window"]["TARGET_FPS"] = std::to_string(fps);
	}
};

#endif