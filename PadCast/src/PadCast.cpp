#include "PadCast.h"
#include "menus.h"

#include <chrono>
#include <thread>
#include <print>

int main()
{
    // ----- Setup ----- //
    Config mainConfig{};
    MenuContext menu;

    // Window setup -- this must happen before instantiating GamepadDisplay
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(
        mainConfig.getInitWinWidth(), 
        mainConfig.getInitWinHeight(),
        "PadCast"
    );
    window.SetTargetFPS(mainConfig.getFPS());

    GamepadDisplay display{ mainConfig };

    // short pause to allow for controller detection
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (mainConfig.getDebugMode())
    {
        SetTraceLogLevel(LOG_ALL);
    }

    // display = gamepad stuff, will refactor later
    MenuContext::MenuParams menuParams(menu, window, mainConfig, display);
    // ----- *** ----- //
  
    // ----- Cached values ----- //
    // Track window dimensions
    int lastWinWidth{ window.GetWidth() };
    int lastWinHeight{ window.GetHeight() };
    bool winDimensionsChanged{ false };
    // Cache canvas dimensions
    const int canvasWidth{ mainConfig.getImgCanvasWidth() };
    const int canvasHeight{ mainConfig.getImgCanvasHeight() };
    // Gamepad connection counter stuff
    static int gamepadCheckCounter = 0;
    bool gamepadConnected{ false };
    // ----- *** ----- //

    // ----- Main Loop ----- //
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
        window.ClearBackground(display.getBGColor());

        ScalingInfo scaling(currentWidth, currentHeight, canvasWidth, canvasHeight);

        // Handles accessing menu and menu navigation
        HandleMenuInput(menuParams, scaling);

        // Draw base controller
        display.getTextures().unpressed.Draw(
            raylib::Vector2{ scaling.offsetX, scaling.offsetY },
            0.0f,
            scaling.scale,
            raylib::WHITE
        );

        // Check gamepad connection
        if (++gamepadCheckCounter >= 10)
        {
            gamepadCheckCounter = 0;
            gamepadConnected = display.updateGamepadConnection(raylib::Gamepad::IsAvailable(0));
        }

        // Display gamepad stuff
        if (gamepadConnected)
        {
            raylib::Gamepad gamepad(0);
            display.drawGamepadButtons(gamepad, scaling);
        }
        else
        {
            display.drawNoGamepadMessage(scaling);
        }

        // If menu is active, draw the menu
        if (menu.active != Menu::None)
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

    mainConfig.SaveConfig();

    return 0;
}