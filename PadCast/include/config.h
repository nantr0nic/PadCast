#ifndef PADCAST_CONFIG_H
#define PADCAST_CONFIG_H
#define MINI_CASE_SENSITIVE

#include "mini/ini.h"
#include "pathmanager.h"

#include <filesystem>
#include <string>
#include <iostream>

class Config
{
private:
	std::string mConfigPath{};
	mINI::INIFile mConfigFile{ mConfigPath };
	mINI::INIStructure config_ini;

	struct DefaultValues
	{
		// Window defaults
		static constexpr int INITIAL_WINDOW_WIDTH{ 960 };
		static constexpr int INITIAL_WINDOW_HEIGHT{ 540 };
		static constexpr int TARGET_FPS{ 60 };
		static constexpr int USE_VSYNC{ 0 };
		// Background color defaults
		static constexpr int BACKGROUND_COLOR{ 0 };
		static constexpr int CUSTOM_BG_RED{ 0 };
		static constexpr int CUSTOM_BG_GREEN{ 0 };
		static constexpr int CUSTOM_BG_BLUE{ 0 };
		static constexpr int USE_CUSTOM_BG{ 0 };
		// Image defaults
		static constexpr int IMAGE_CANVAS_WIDTH{ 960 };
		static constexpr int IMAGE_CANVAS_HEIGHT{ 540 };
		static constexpr int USE_CUSTOM_TINT{ 0 };
		static constexpr int IMAGE_TINT_RED{ 255 };
		static constexpr int IMAGE_TINT_GREEN{ 255 };
		static constexpr int IMAGE_TINT_BLUE{ 255 };
		static constexpr int IMAGE_TINT_PALETTE{ 0 };
		// Gamepad defaults
		static constexpr int STABILITY_THRESHOLD{ 5 };
		static constexpr int GAMEPAD_INDEX{ 0 };
		// Font defaults
		static constexpr int MIN_FONT_SIZE{ 10 };
		static constexpr int DEFAULT_FONT_SIZE{ 35 };
		static constexpr int TEXT_OFFSET{ 50 };
		// Debug defaults
		static constexpr int DEBUG_MODE{ 0 };
	};

	struct SNESMapDefaults
	{
		// D-Pad ("Left face")
		static constexpr int DPAD_UP{ 1 };
		static constexpr int DPAD_RIGHT{ 2 };
		static constexpr int DPAD_DOWN{ 3 };
		static constexpr int DPAD_LEFT{ 4 };
		// Buttons ("Right face")
		static constexpr int X_BUTTON{ 5 };
		static constexpr int A_BUTTON{ 6 };
		static constexpr int B_BUTTON{ 7 };
		static constexpr int Y_BUTTON{ 8 };
		// Shoulder buttons  
		static constexpr int L_BUTTON{ 9 };
		static constexpr int R_BUTTON{ 11 };
		// System buttons
		static constexpr int SELECT{ 13 };
		static constexpr int START{ 15 };
	};

public:
	Config() { loadConfig(); }
	~Config() { saveConfig(); }

	//$ ----- config.ini functions (load, save, etc.) ----- //
	// this assumes that the executable is in the root directory
	std::string getConfigFilePath() const
	{
		return PathManager::getConfigFilePath();
	}

	void loadConfig();

	void validateConfig();

	bool hasValue(const std::string& section, const std::string& key) const
	{
		if (!config_ini.has(section))
		{
			return false;
		}
		auto sectionData{ config_ini.get(section) };
		return sectionData.has(key);
	}

	bool saveConfig()
	{
		return mConfigFile.write(config_ini);
	}

	void reloadConfig()
	{
		mConfigFile.read(config_ini);
		validateConfig();
	}

	//$ ----- getters ----- //
	// so far all the values are ints, so we'll just keep this function
	// but it would be fun to write a template at a later time :)
	auto& getIni() const { return config_ini; }

	int getValue(const std::string& section, const std::string& key) const
	{
		if (!hasValue(section, key))
		{
			std::cout << section << " or " << key << " doesn't exist, setting with default value." << std::endl;
			return getDefault(section, key);
		}
		try
		{
			return std::stoi(config_ini.get(section).get(key));
		}
		catch (const std::invalid_argument&)
		{
			std::cerr << "Wrong value type " << key << " for " << section << ", using default" << std::endl;
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
		return getValue("Image", "IMAGE_CANVAS_WIDTH");
	}
	int getImgCanvasHeight() const
	{
		return getValue("Image", "IMAGE_CANVAS_HEIGHT");
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
	int getVSYNC() const
	{
		return getValue("Window", "USE_VSYNC");
	}
	int getBGColor() const
	{
		return getValue("Window", "BACKGROUND_COLOR");
	}
	int getGPIndex() const
	{
		return getValue("Gamepad", "GAMEPAD_INDEX");
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
	void updateUseVSYNC(int vsync)
	{
		config_ini["Window"]["USE_VSYNC"] = std::to_string(vsync);
	}
	void updateBGColor(int background_int)
	{
		config_ini["Window"]["BACKGROUND_COLOR"] = std::to_string(background_int);
	}
	void updateUseCustomBG(int useCustom)
	{
		config_ini["Window"]["USE_CUSTOM_BG"] = std::to_string(useCustom);
	} //*
	void updateButtonConfig(const std::string& key, int new_button)
	{
		config_ini["ButtonMap"][key] = std::to_string(new_button);
	}
	void updateUseCustomTint(int useCustom)
	{
		config_ini["Image"]["USE_CUSTOM_TINT"] = std::to_string(useCustom);
	}
	void updateImageTintPalette(int paletteIndex)
	{
		config_ini["Image"]["IMAGE_TINT_PALETTE"] = std::to_string(paletteIndex);
	}
	void updateGamepadIndex(int gpIndex)
	{
		config_ini["Gamepad"]["GAMEPAD_INDEX"] = std::to_string(gpIndex);
	}

//$ ----- Reset -----
	void resetButtonMap();
};

#endif