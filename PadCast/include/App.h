#ifndef PADCAST_APP_H
#define PADCAST_APP_H

#ifdef _WIN32
	#define NOGDI
	#define NOUSER
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#endif

#include "config.h"
#include "PadCast.h"
#include "scaling.h"
#include "menus.h"

#include <raylib-cpp.hpp>

#ifdef _WIN32
	#undef NOGDI
	#undef NOUSER
#endif

class App
{
public:
	explicit App(Config& config);
	~App();

	void run();

private:
	raylib::Window createWindow();

	// External reference
	Config& mConfig;

	// Owned objects (construction order follows declaration)
	raylib::Window mWindow;
	PadCast mPadcast;
	MenuContext mMenu;

	// Cached dimensions
	int mCanvasWidth;
	int mCanvasHeight;

	// Window dimension tracking
	int mLastWinWidth;
	int mLastWinHeight;
	bool mWinDimensionsChanged{ false };

	// Gamepad polling state
	int mGamepadCheckCounter{ 0 };
	int mGamepadIndex;
	bool mGamepadConnected{ false };

	// Per-frame scaling (updated every frame in run())
	ScalingInfo mScaling;

	// Reference aggregator for menu system
	MenuContext::MenuParams mMenuParams;
};

#endif
