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

	const GamepadTextures& getTextures() const { return mTextures; }
	Config& getConfig() { return mConfig; }
	bool isDebugOn() const { return mDebugMode; }

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
	void invalidateBGCache() { mBGCacheValid = false; }
	raylib::Color getBGColor() const;

	// Tint
	void invalidateTintCache() { mTintCacheValid = false; }

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
		mButtonCache.refreshCache(mButtonMap);
	}

private:
	GamepadTextures mTextures;
	Config& mConfig;
	ButtonMap mButtonMap;
	CachedButtons mButtonCache;

	bool mGamepadWasConnected{ false };
	int mStabilityCounter{ 0 };

	// Cache values for optimization
	mutable Color mCachedBGColor{ BLACK };
	mutable int mLastBGColorValue{ -1 };
	mutable int mLastUseCustomBG{ -1 };
	mutable int mLastCustomRed{ -1 };
	mutable int mLastCustomGreen{ -1 };
	mutable int mLastCustomBlue{ -1 };
	mutable int mCachedStabilityThreshold{ -1 };
	// Custom BG Color cache
	mutable int mCachedUseCustomBG{ -1 };
	mutable int mCachedCustomRed{ -1 };
	mutable int mCachedCustomGreen{ -1 };
	mutable int mCachedCustomBlue{ -1 };
	mutable int mCachedBGColorValue{ -1 };
	mutable bool mBGCacheValid{ false };
	// Custom pressed tint cache
	mutable bool mTintCacheValid{ false };
	mutable int mCachedUseCustomTint{ -1 };
	mutable int mCachedTintR{ -1 };
	mutable int mCachedTintG{ -1 };
	mutable int mCachedTintB{ -1 };
	mutable Color mCachedPressedTint{ WHITE };

	bool mDebugMode{ false };
};

#endif