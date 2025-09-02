#ifdef _WIN32
    #define NOGDI
    #define NOUSER
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif

#include "PadCast.h"
#include "config.h"
#include "menus.h"

#ifdef _WIN32
    #undef NOGDI
    #undef NOUSER
#endif

#include <thread>
#include <chrono>

int main()
{
	//$ ----- Setup ----- //
	Config mainConfig{};

	// Window setup
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	raylib::Window window {
		mainConfig.getInitWinWidth(),
		mainConfig.getInitWinHeight(),
		"PadCast"
	};
    raylib::Image icon(PathManager::getResourcePath("padcast.png"));
    window.SetIcon(icon);

	window.SetTargetFPS(mainConfig.getFPS());

	PadCast padcast{ mainConfig };
	MenuContext menu;

	// short pause to allow for controller detection
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	if (mainConfig.getDebugMode())
	{
		SetTraceLogLevel(LOG_ALL);
	}
	// ----- ***** ----- //

	//$ ----- Cached values ----- //
	// Track window dimensions
	int lastWinWidth{ window.GetWidth() };
	int lastWinHeight{ window.GetHeight() };
	bool winDimensionsChanged{ false };
	// Cache canvas dimensions
	const int canvasWidth{ mainConfig.getImgCanvasWidth() };
	const int canvasHeight{ mainConfig.getImgCanvasHeight() };
	// Gamepad connection
	static int gamepadCheckCounter = 0;
    static int gamepadIndex{ mainConfig.getGPIndex() };
	bool gamepadConnected{ false };
	// ----- ***** ----- //

	ScalingInfo scaling{ window.GetWidth(), window.GetHeight(), canvasWidth, canvasHeight };
	MenuContext::MenuParams menuParams{ menu, window, mainConfig, padcast, scaling, gamepadIndex};

	//$ ----- Main Loop ----- //
    while (!window.ShouldClose())
    {
        int currentWidth = window.GetWidth();
        int currentHeight = window.GetHeight();

        // Check and update window dimensions if necessary
        if (currentWidth != lastWinWidth || currentHeight != lastWinHeight)
        {
            mainConfig.updateWindowSize(currentWidth, currentHeight);
            lastWinWidth = currentWidth;
            lastWinHeight = currentHeight;
            winDimensionsChanged = true;
        }

        window.BeginDrawing();
        window.ClearBackground(padcast.getBGColor());

        // Update scaling each frame
        scaling = ScalingInfo(currentWidth, currentHeight, canvasWidth, canvasHeight);
        menuParams.scaling = scaling;

        // Handles accessing menu and menu navigation
        HandleMenuInput(menuParams);

        // Draw base controller
        padcast.getTextures().unpressed.Draw(
            raylib::Vector2{ scaling.offsetX, scaling.offsetY },
            0.0f,
            scaling.scale,
            raylib::WHITE
        );

        // Check gamepad connection
        if (++gamepadCheckCounter >= 15)
        {
            gamepadCheckCounter = 0;
            gamepadConnected = padcast.updateGamepadConnection(
                                       raylib::Gamepad::IsAvailable(gamepadIndex)
                               );
        }

        // Display gamepad stuff
        if (gamepadConnected && (menu.active != Menu::RemapButtons))
        {
            raylib::Gamepad gamepad(gamepadIndex);
            padcast.drawGamepadButtons(gamepad, scaling);
        }
        else
        {
            padcast.drawNoGamepadMessage(scaling);
        }

        // Add remap screen handling here to avoid lambda insanity
        if (menu.active == Menu::RemapButtons)
        {
            RemapButtonScreens(menuParams);
        }
        else if (menu.active != Menu::None)
        {
            DrawMenu(menu, scaling, mainConfig, 50, 50);
        }

        window.EndDrawing();
    }

    // If window dimensions changed from last open, update initial dimensions
    if (winDimensionsChanged)
    {
        mainConfig.updateInitWinSizes();
    }

    mainConfig.saveConfig();

    return 0;
}