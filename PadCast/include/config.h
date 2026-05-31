#ifndef PADCAST_CONFIG_H
#define PADCAST_CONFIG_H
#define MINI_CASE_SENSITIVE

#include "mini/ini.h"
#include "pathmanager.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <iostream>

class Config
{
private:
	std::string mConfigPath{};
	mINI::INIFile mConfigFile{ mConfigPath };
	mINI::INIStructure config_ini;
	bool mNeedsSave{ false };

	// Template helper: validates an int config key, resetting to default if missing or invalid.
	// Sets mNeedsSave to true when a reset occurs.
	template<typename Validator>
	void validateInt(const std::string& section, const std::string& key, int defaultValue, Validator&& isValid)
	{
		if (!hasValue(section, key))
		{
			config_ini[section][key] = std::to_string(defaultValue);
			mNeedsSave = true;
			return;
		}

		int currentVal = getValue(section, key);
		if (!isValid(currentVal))
		{
			config_ini[section][key] = std::to_string(defaultValue);
			mNeedsSave = true;
		}
	}

	// Template helper: sets a config value and marks the config as needing save.
	template<typename T>
	void setValue(const std::string& section, const std::string& key, T value)
	{
		config_ini[section][key] = std::to_string(value);
		mNeedsSave = true;
	}

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
		static constexpr int DRAW_JOYSTICK_IDLE{ 1 };  // 1=always, 0=only when moving
		// Gamepad defaults
		static constexpr int STABILITY_THRESHOLD{ 5 };
		static constexpr int GAMEPAD_INDEX{ 0 };
		static constexpr int LAYOUT{ 0 };  // 0=SNES, 1=N64
		static constexpr int STICK_X_AXIS{ 0 };
		static constexpr int STICK_Y_AXIS{ 1 };
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

	struct N64MapDefaults
	{
		// D-Pad
		static constexpr int DPAD_UP{ 1 };
		static constexpr int DPAD_RIGHT{ 2 };
		static constexpr int DPAD_DOWN{ 3 };
		static constexpr int DPAD_LEFT{ 4 };
		// C buttons (user-verified indices)
		static constexpr int C_UP{ 15 };
		static constexpr int C_RIGHT{ 13 };
		static constexpr int C_DOWN{ 5 };
		static constexpr int C_LEFT{ 8 };
		// Face buttons
		static constexpr int A_BUTTON{ 9 };
		static constexpr int B_BUTTON{ 11 };
		// Shoulder / trigger
		static constexpr int L_BUTTON{ 13 };
		static constexpr int R_BUTTON{ 14 };
		static constexpr int Z_BUTTON{ 15 };  // Axis 4 (-1..1) — handled separately in draw
		// System buttons
		static constexpr int START{ 0 };  // 0 = unmapped (N64 Start not detected by this adapter)
	};

public:
	//$ ----- Controller layout enum ----- //
	enum class ControllerLayout
	{
		SNES,
		N64
	};

	static std::string buttonSectionName(ControllerLayout layout)
	{
		switch (layout)
		{
			case ControllerLayout::SNES: return "SNES_ButtonMap";
			case ControllerLayout::N64:  return "N64_ButtonMap";
		}
		return "SNES_ButtonMap";
	}

	static ControllerLayout layoutFromInt(int val)
	{
		switch (val)
		{
			case 0:  return ControllerLayout::SNES;
			case 1:  return ControllerLayout::N64;
			// future: case 2: return ControllerLayout::GameCube;
			default: return ControllerLayout::SNES;
		}
	}

	static std::string resourcesSubdir(ControllerLayout layout)
	{
		switch (layout)
		{
			case ControllerLayout::SNES: return "SNES";
			case ControllerLayout::N64:  return "N64";
		}
		return "SNES";
	}

	//$ ----- Button config key enum (future: layout-aware) ----- //
	// When adding controller layouts, extend this enum or create per-layout
	// mappings that translate these logical buttons to the correct INI keys.
	enum class ButtonConfigKey
	{
		DPAD_UP,
		DPAD_RIGHT,
		DPAD_DOWN,
		DPAD_LEFT,
		X_BUTTON,
		A_BUTTON,
		B_BUTTON,
		Y_BUTTON,
		L_BUTTON,
		R_BUTTON,
		SELECT,
		START,

		// N64-specific
		C_UP,
		C_RIGHT,
		C_DOWN,
		C_LEFT,
		Z_BUTTON,
	};

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
		return config_ini.get(section).has(key);
	}

	bool saveConfig()
	{
		if (!mNeedsSave)
		{
			return true;
		}
		mNeedsSave = false;
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
	// All setters delegate to setValue() to avoid duplication
	// and automatically track mNeedsSave.
	void updateWindowSize(int width, int height)
	{
		setValue("Window", "CURRENT_WINDOW_WIDTH", width);
		setValue("Window", "CURRENT_WINDOW_HEIGHT", height);
	}
	void updateInitWinSizes()
	{
		config_ini["Window"]["INITIAL_WINDOW_WIDTH"] = config_ini["Window"]["CURRENT_WINDOW_WIDTH"];
		config_ini["Window"]["INITIAL_WINDOW_HEIGHT"] = config_ini["Window"]["CURRENT_WINDOW_HEIGHT"];
		mNeedsSave = true;
	}
	void updateTargetFPS(int fps)
	{
		setValue("Window", "TARGET_FPS", fps);
	}
	void updateUseVSYNC(int vsync)
	{
		setValue("Window", "USE_VSYNC", vsync);
	}
	void updateBGColor(int backgroundColor)
	{
		setValue("Window", "BACKGROUND_COLOR", backgroundColor);
	}
	void updateUseCustomBG(int useCustom)
	{
		setValue("Window", "USE_CUSTOM_BG", useCustom);
	}
	void updateButtonConfig(ButtonConfigKey button, int newButtonIndex,
	                         ControllerLayout layout = ControllerLayout::SNES)
	{
		const char* keyName = nullptr;
		switch (button)
		{
			case ButtonConfigKey::DPAD_UP:    keyName = "DPAD_UP"; break;
			case ButtonConfigKey::DPAD_RIGHT: keyName = "DPAD_RIGHT"; break;
			case ButtonConfigKey::DPAD_DOWN:  keyName = "DPAD_DOWN"; break;
			case ButtonConfigKey::DPAD_LEFT:  keyName = "DPAD_LEFT"; break;
			case ButtonConfigKey::X_BUTTON:   keyName = "X_BUTTON"; break;
			case ButtonConfigKey::A_BUTTON:   keyName = "A_BUTTON"; break;
			case ButtonConfigKey::B_BUTTON:   keyName = "B_BUTTON"; break;
			case ButtonConfigKey::Y_BUTTON:   keyName = "Y_BUTTON"; break;
			case ButtonConfigKey::L_BUTTON:   keyName = "L_BUTTON"; break;
			case ButtonConfigKey::R_BUTTON:   keyName = "R_BUTTON"; break;
			case ButtonConfigKey::SELECT:     keyName = "SELECT"; break;
			case ButtonConfigKey::START:      keyName = "START"; break;
			case ButtonConfigKey::C_UP:       keyName = "C_UP"; break;
			case ButtonConfigKey::C_RIGHT:    keyName = "C_RIGHT"; break;
			case ButtonConfigKey::C_DOWN:     keyName = "C_DOWN"; break;
			case ButtonConfigKey::C_LEFT:     keyName = "C_LEFT"; break;
			case ButtonConfigKey::Z_BUTTON:   keyName = "Z_BUTTON"; break;
		}

		if (keyName)
			setValue(buttonSectionName(layout), keyName, newButtonIndex);
	}
	void updateUseCustomTint(int useCustom)
	{
		setValue("Image", "USE_CUSTOM_TINT", useCustom);
	}
	void updateDrawJoystickIdle(int val)
	{
		setValue("Image", "DRAW_JOYSTICK_IDLE", val);
	}
	int getDrawJoystickIdle() const
	{
		return getValue("Image", "DRAW_JOYSTICK_IDLE");
	}
	void updateImageTintPalette(int paletteIndex)
	{
		setValue("Image", "IMAGE_TINT_PALETTE", paletteIndex);
	}
	void updateGamepadIndex(int gpIndex)
	{
		setValue("Gamepad", "GAMEPAD_INDEX", gpIndex);
	}
	void updateLayout(int layoutVal)
	{
		setValue("Gamepad", "LAYOUT", layoutVal);
	}
	int getLayout() const
	{
		return getValue("Gamepad", "LAYOUT");
	}
	int getStickXAxis() const { return getValue("Gamepad", "STICK_X_AXIS"); }
	int getStickYAxis() const { return getValue("Gamepad", "STICK_Y_AXIS"); }
	void updateStickXAxis(int val) { setValue("Gamepad", "STICK_X_AXIS", val); }
	void updateStickYAxis(int val) { setValue("Gamepad", "STICK_Y_AXIS", val); }

	//$ ----- Button Map Loader ----- //
	// Reads button mappings for a given layout from the INI section
	// returned by buttonSectionName(layout). Returns a map of INI key
	// name → integer value. Caller maps key names to raylib constants.
	std::unordered_map<std::string, int> loadButtonMapping(ControllerLayout layout) const;

//$ ----- Reset -----
	void resetButtonMap(ControllerLayout layout = ControllerLayout::SNES);
};

#endif