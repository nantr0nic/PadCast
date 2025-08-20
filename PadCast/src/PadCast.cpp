#include "PadCast.h"
#include "menus.h"

#include <chrono>
#include <thread>
#include <print>

int main()
{
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

    while (!window.ShouldClose())
    {
        window.BeginDrawing();
        window.ClearBackground(display.getBGColor());

        ScalingInfo scaling(
            window.GetWidth(),
            window.GetHeight(),
            mainConfig.getImgCanvasWidth(),
            mainConfig.getImgCanvasHeight()
        );

        // Handles accessing menu and menu navigation
        HandleMenuInput(menu, window, mainConfig, scaling);

        // Draw base controller
        display.getTextures().unpressed.Draw(
            raylib::Vector2{ scaling.offsetX, scaling.offsetY },
            0.0f,
            scaling.scale,
            raylib::WHITE
        );

        // Handle gamepad input
        bool gamepadConnected = display.updateGamepadConnection(raylib::Gamepad::IsAvailable(0));

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

        // If user resized window, update config with new dimensions
        if (mainConfig.getCurrentWinHeight() != window.GetHeight() 
            || mainConfig.getCurrentWinWidth() != window.GetWidth())
        {
            mainConfig.updateWindowSize(window.GetWidth(), window.GetHeight());
        }

        window.EndDrawing();
    }

    // If window dimensions changed from last open, update initial dimensions
    if (mainConfig.getCurrentWinHeight() != mainConfig.getInitWinHeight()
        || mainConfig.getCurrentWinWidth() != mainConfig.getInitWinWidth())
    {
        mainConfig.updateInitWinSizes();
    }

    mainConfig.SaveConfig();

    return 0;
}