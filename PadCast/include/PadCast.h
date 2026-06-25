#ifndef PADCAST_H
#define PADCAST_H

#ifdef _WIN32
	#define NOGDI             // hide GDI: Rectangle, DrawText, etc.
	#define NOUSER            // hide USER32: CloseWindow, ShowCursor, etc.
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#endif

#include <raylib.h>

#ifdef _WIN32
	#undef NOGDI
	#undef NOUSER
#endif

#include <raylib-cpp.hpp>
#include "config.h"
#include "scaling.h"
#include <unordered_map>

struct GamepadTextures
{
	raylib::Texture2D unpressed;
	raylib::Texture2D pressedA;
	raylib::Texture2D pressedB;
	raylib::Texture2D pressedX;
	raylib::Texture2D pressedY;
	raylib::Texture2D pressedUp;
	raylib::Texture2D pressedLeft;
	raylib::Texture2D pressedDown;
	raylib::Texture2D pressedRight;
	raylib::Texture2D pressedStart;
	raylib::Texture2D pressedSelect;
	raylib::Texture2D pressedLBump;
	raylib::Texture2D pressedRBump;
	// N64-specific overlays
	raylib::Texture2D pressedCUp;
	raylib::Texture2D pressedCDown;
	raylib::Texture2D pressedCLeft;
	raylib::Texture2D pressedCRight;
	raylib::Texture2D pressedZ;
	raylib::Texture2D pressedJoystick;

	explicit GamepadTextures(Config::ControllerLayout layout = Config::ControllerLayout::SNES);
	void load(Config::ControllerLayout layout);
};

enum class BackgroundColor
{
	Black, 
	White, 
	Red, 
	Green, 
	Blue
};

struct ButtonMap
{
	std::unordered_map<int, int> buttonIndex;
	std::unordered_map<int, int> defaultSNESIndex {
		{GAMEPAD_BUTTON_LEFT_FACE_UP, 1},    // D-pad UP
		{GAMEPAD_BUTTON_LEFT_FACE_RIGHT, 2}, // D-pad RIGHT
		{GAMEPAD_BUTTON_LEFT_FACE_DOWN, 3},  // D-pad DOWN
		{GAMEPAD_BUTTON_LEFT_FACE_LEFT, 4},  // D-pad LEFT
		{GAMEPAD_BUTTON_RIGHT_FACE_UP, 5},   // X
		{GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, 6},// A
		{GAMEPAD_BUTTON_RIGHT_FACE_DOWN, 7}, // B
		{GAMEPAD_BUTTON_RIGHT_FACE_LEFT, 8}, // Y
		{GAMEPAD_BUTTON_LEFT_TRIGGER_1, 9},  // Left shoulder
		{GAMEPAD_BUTTON_RIGHT_TRIGGER_1, 11},// Right shoulder
		{GAMEPAD_BUTTON_MIDDLE_LEFT, 13},    // Select
		{GAMEPAD_BUTTON_MIDDLE_RIGHT, 15}    // Start
	};

	std::unordered_map<int, int> defaultN64Index {
		{GAMEPAD_BUTTON_LEFT_FACE_UP, 1},     // D-pad UP
		{GAMEPAD_BUTTON_LEFT_FACE_RIGHT, 2},  // D-pad RIGHT
		{GAMEPAD_BUTTON_LEFT_FACE_DOWN, 3},   // D-pad DOWN
		{GAMEPAD_BUTTON_LEFT_FACE_LEFT, 4},   // D-pad LEFT
		{GAMEPAD_BUTTON_RIGHT_FACE_UP, 15},   // C UP
		{GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, 13},// C RIGHT
		{GAMEPAD_BUTTON_RIGHT_FACE_DOWN, 8},  // C DOWN
		{GAMEPAD_BUTTON_RIGHT_FACE_LEFT, 5},  // C LEFT
		{GAMEPAD_BUTTON_LEFT_TRIGGER_1, 9},   // L button
		{GAMEPAD_BUTTON_RIGHT_TRIGGER_1, 11}, // R button
		{GAMEPAD_BUTTON_MIDDLE_LEFT, 7},      // A button
		{GAMEPAD_BUTTON_MIDDLE_RIGHT, 6}      // B button
	};

	ButtonMap()
	{
		// std::println("DEBUG: ButtonMap constructor - buttonIndex starts empty");
	}

	void remapButton(int raylibButton, int newIndex)
	{
		buttonIndex[raylibButton] = newIndex;
	}
};

struct CachedButtons
{
	// Cache all frequently used button indices
	int dpadUp{};
	int dpadRight{};
	int dpadDown{};
	int dpadLeft{};
	int xButton{};        // RIGHT_FACE_UP
	int aButton{};        // RIGHT_FACE_RIGHT
	int bButton{};        // RIGHT_FACE_DOWN
	int yButton{};        // RIGHT_FACE_LEFT
	int leftTrigger{};
	int rightTrigger{};
	int selectButton{};   // MIDDLE_LEFT
	int startButton{};    // MIDDLE_RIGHT
	// N64-specific
	int cUp{};
	int cDown{};
	int cLeft{};
	int cRight{};
	int zButton{};

	CachedButtons() {}; // empty default

	CachedButtons(const ButtonMap& buttonMap)
	{
		refreshCache(buttonMap);
	}

	void refreshCache(const ButtonMap& buttonMap,
	                   Config::ControllerLayout layout = Config::ControllerLayout::SNES);
};

class PadCast
{
public:
	explicit PadCast(Config& mainConfig);
	~PadCast() = default;

	const GamepadTextures& getTextures() const { return mTextures; }
	Config& getConfig() { return mConfig; }
	bool isDebugOn() const { return mDebugMode; }

public:
	// Gamepad functions
	bool updateGamepadConnection(bool currentlyAvailable);
	void drawGamepadButtons(const raylib::Gamepad& gamepad, const ScalingInfo& scaling);
	void drawNoGamepadMessage(const ScalingInfo& scaling);
	// USB gamepads seem to be 0 by default on Windows, might require finding on Linux
	void findGamepadIndex();
	std::string getGamepadName(int i) { raylib::Gamepad gamepad(i); return gamepad.GetName(); }
	int getGamepadIndex() { return gamepadIndex; }
	void setGamepadIndex(int i) { gamepadIndex = i;  mConfig.updateGamepadIndex(i); }
	raylib::Gamepad getGamepad() const { return raylib::Gamepad{ gamepadIndex }; }

	// Controller layout
	Config::ControllerLayout getCurrentLayout() const { return mCurrentLayout; }
	void setCurrentLayout(Config::ControllerLayout layout)
	{
		mCurrentLayout = layout;
		mConfig.updateLayout(layout == Config::ControllerLayout::N64 ? 1 : 0);
		mTextures.load(layout);
		loadButtonsFromConfig();
	}

	// Gamepad debug functions
	void drawDebugButtonIndex(const raylib::Gamepad& gamepad, const ScalingInfo& scaling);
	void debugGamepadInfo(const raylib::Gamepad& gamepad);

	// Background color functions
	bool isValidBackgroundColor(int value) const
	{
		return (value >= 0 && value <= static_cast<int>(BackgroundColor::Blue));
	}
	raylib::Color getBGColor() const;

	// Button Map Functions
	void loadButtonsFromConfig();
	void resetButtonsToDefault()
	{
		mButtonMap.buttonIndex = mButtonMap.defaultSNESIndex;
		mButtonCache.refreshCache(mButtonMap);
	}
	void refreshButtonCache() { mButtonCache.refreshCache(mButtonMap); }
	void setButtonMap(int raylibButton, int newIndex)
	{
		mButtonMap.remapButton(raylibButton, newIndex);
		mButtonCache.refreshCache(mButtonMap, mCurrentLayout);
	}

private:
	GamepadTextures mTextures;
	Config& mConfig;
	ButtonMap mButtonMap;
	CachedButtons mButtonCache;

	bool mGamepadWasConnected{ false };
	int mStabilityCounter{ 0 };
	int gamepadIndex{ 0 };
	Config::ControllerLayout mCurrentLayout{ Config::ControllerLayout::SNES };

	// Cache values for optimization
	mutable int mCachedStabilityThreshold{ -1 };

	bool mDebugMode{ false };
};

#endif