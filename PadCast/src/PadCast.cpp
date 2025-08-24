#include "PadCast.h"
#include "menus.h"

#include <chrono>
#include <thread>

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

    // initial scaling
    ScalingInfo scaling(window.GetWidth(), window.GetHeight(), canvasWidth, canvasHeight);
    // display = gamepad stuff, will refactor later
    MenuContext::MenuParams menuParams(menu, window, mainConfig, display, scaling);

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

        // Update scaling each frame
        scaling = ScalingInfo(currentWidth, currentHeight, canvasWidth, canvasHeight);
        menuParams.scaling = scaling;

        // Handles accessing menu and menu navigation
        HandleMenuInput(menuParams);

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
        if (gamepadConnected && (menu.active != Menu::RemapButtons))
        {
            raylib::Gamepad gamepad(0);
            display.drawGamepadButtons(gamepad, scaling);
        }
        else
        {
            display.drawNoGamepadMessage(scaling);
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

    mainConfig.SaveConfig();

    return 0;
}