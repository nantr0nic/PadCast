#ifndef PADCAST_H
#define PADCAST_H

#include <raylib.h>
#include <raylib-cpp.hpp>
#include "config.h"

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

	GamepadTextures();
};

struct ScalingInfo
{
	float scale{};
	float offsetX{};
	float offsetY{};

	ScalingInfo(int currentWidth, int currentHeight, int originalWidth, int originalHeight);
};

enum class BackgroundColor
{
	// 0     1     2     3     4
	Black, White, Red, Green, Blue
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

	CachedButtons() {}; // empty default

	CachedButtons(const ButtonMap& buttonMap)
	{
		refreshCache(buttonMap);
	}

	void refreshCache(const ButtonMap& buttonMap);
};

class PadCast
{
public:
	explicit PadCast(Config& mainConfig);
	~PadCast() = default;

	const GamepadTextures& getTextures() const { return textures; }
	Config& getConfig() { return config; }
	bool isDebugOn() const { return debugMode; }

public:
	// Gamepad display functions
	bool updateGamepadConnection(bool currentlyAvailable);
	void drawGamepadButtons(const raylib::Gamepad& gamepad, const ScalingInfo& scaling);
	void drawNoGamepadMessage(const ScalingInfo& scaling);
	void drawDebugButtonIndex(const raylib::Gamepad& gamepad, const ScalingInfo& scaling);

	// Background color functions
	bool isValidBackgroundColor(int value) const
	{
		return (value >= 0 && value <= static_cast<int>(BackgroundColor::Blue));
	}
	void invalidateBGCache() { configCacheValid = false; }
	raylib::Color getBGColor() const;

	// Tint
	void invalidateTintCache() { tintCacheValid = false; }

	// Button Map Functions
	void loadButtonsFromConfig();
	void resetButtonsToDefault()
	{
		buttonMap.buttonIndex = buttonMap.defaultSNESIndex;
		buttonCache.refreshCache(buttonMap);
	}
	void refreshButtonCache() { buttonCache.refreshCache(buttonMap); }
	void setButtonMap(int raylibButton, int newIndex)
	{
		buttonMap.remapButton(raylibButton, newIndex);
		buttonCache.refreshCache(buttonMap);
	}

private:
	GamepadTextures textures;
	Config& config;
	ButtonMap buttonMap;
	CachedButtons buttonCache;

	bool gamepadWasConnected{ false };
	int stabilityCounter{ 0 };

	// Cache values for optimization
	mutable Color cachedBGColor{ BLACK };
	mutable int lastBGColorValue{ -1 };
	mutable int lastUseCustomBG{ -1 };
	mutable int lastCustomRed{ -1 };
	mutable int lastCustomGreen{ -1 };
	mutable int lastCustomBlue{ -1 };
	mutable int cachedStabilityThreshold{ -1 };
	// Custom BG Color cache
	mutable int cachedUseCustomBG{ -1 };
	mutable int cachedCustomRed{ -1 };
	mutable int cachedCustomGreen{ -1 };
	mutable int cachedCustomBlue{ -1 };
	mutable int cachedBGColorValue{ -1 };
	mutable bool configCacheValid{ false };
	// Custom pressed tint cache
	mutable bool tintCacheValid{ false };
	mutable int cachedUseCustomTint{ -1 };
	mutable int cachedTintR{ -1 };
	mutable int cachedTintG{ -1 };
	mutable int cachedTintB{ -1 };
	mutable Color cachedTextureTint{ WHITE };

	bool debugMode{ false };
};

#endif