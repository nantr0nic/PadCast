#include "App.h"
#include "pathmanager.h"

// Uncomment to benchmark: #include "benchmark.h" 
// and add FrameTimer + recordFrame() in run()
// instructions are in benchmark.h

#include <thread>
#include <chrono>

raylib::Window App::createWindow()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	return raylib::Window{
		mConfig.getInitWinWidth(),
		mConfig.getInitWinHeight(),
		"PadCast"
	};
}

App::App(Config& config)
	: mConfig{ config }
	, mWindow{ createWindow() }
	, mPadcast{ mConfig }
	, mCanvasWidth{ mConfig.getImgCanvasWidth() }
	, mCanvasHeight{ mConfig.getImgCanvasHeight() }
	, mLastWinWidth{ mWindow.GetWidth() }
	, mLastWinHeight{ mWindow.GetHeight() }
	, mGamepadIndex{ mConfig.getGPIndex() }
	, mScaling{ mWindow.GetWidth(), mWindow.GetHeight(), mCanvasWidth, mCanvasHeight }
	, mMenuParams{ mMenu, mWindow, mConfig, mPadcast, mScaling, mGamepadIndex }
{
	// Window icon
	raylib::Image icon(PathManager::getResourcePath("padcast.png"));
	mWindow.SetIcon(icon);

	// VSYNC or manual FPS
	if (mConfig.getVSYNC())
	{
		SetWindowState(FLAG_VSYNC_HINT);
		int currentMonitor = GetCurrentMonitor();
		int refreshRate = GetMonitorRefreshRate(currentMonitor);
		mWindow.SetTargetFPS(refreshRate);
	}
	else
	{
		ClearWindowState(FLAG_VSYNC_HINT);
		mWindow.SetTargetFPS(mConfig.getFPS());
	}

	// Short pause to allow for controller detection
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	if (mConfig.getDebugMode())
	{
		SetTraceLogLevel(LOG_ALL);
	}
}

App::~App()
{
	if (mWinDimensionsChanged)
	{
		mConfig.updateInitWinSizes();
	}
	mConfig.saveConfig();
}

void App::run()
{
	while (!mWindow.ShouldClose())
	{
		int currentWidth = mWindow.GetWidth();
		int currentHeight = mWindow.GetHeight();

		// Check and update window dimensions if necessary
		if (currentWidth != mLastWinWidth || currentHeight != mLastWinHeight)
		{
			mConfig.updateWindowSize(currentWidth, currentHeight);
			mLastWinWidth = currentWidth;
			mLastWinHeight = currentHeight;
			mWinDimensionsChanged = true;
		}

		mWindow.BeginDrawing();
		mWindow.ClearBackground(mPadcast.getBGColor());

		// Update scaling each frame
		mScaling = ScalingInfo(currentWidth, currentHeight, mCanvasWidth, mCanvasHeight);

		// Handles accessing menu and menu navigation
		HandleMenuInput(mMenuParams);

		// Draw base controller
		mPadcast.getTextures().unpressed.Draw(
			raylib::Vector2{ mScaling.offsetX, mScaling.offsetY },
			0.0f,
			mScaling.scale,
			raylib::WHITE
		);

		// Check gamepad connection
		if (++mGamepadCheckCounter >= 15)
		{
			mGamepadCheckCounter = 0;
			mGamepadConnected = mPadcast.updateGamepadConnection(
									   raylib::Gamepad::IsAvailable(mGamepadIndex)
							   );
		}

		// Display gamepad stuff
		if (mGamepadConnected && (mMenu.active != Menu::RemapButtons))
		{
			raylib::Gamepad gamepad(mGamepadIndex);
			mPadcast.drawGamepadButtons(gamepad, mScaling);
		}
		else
		{
			mPadcast.drawNoGamepadMessage(mScaling);
		}

		// Add remap screen handling here to avoid lambda insanity
		if (mMenu.active == Menu::RemapButtons)
		{
			RemapButtonScreens(mMenuParams);
		}
		else if (mMenu.active != Menu::None)
		{
			DrawMenu(mMenu, mScaling, mConfig, 50, 50);
		}

		mWindow.EndDrawing();
	}
}
