#include "PadCast.h"
#include "menus.h"

#include <chrono>
#include <thread>
#include <print>

int main()
{
    bool debug_mode{ false };

    Config mainConfig{};
    MenuContext menu;

    // Window setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(
        mainConfig.getInitWinWidth(), 
        mainConfig.getInitWinHeight(),
        "PadCast");
    window.SetTargetFPS(mainConfig.getFPS());

    GamepadDisplay display{ mainConfig };

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (debug_mode)
    {
        SetTraceLogLevel(LOG_ALL);
    }

    while (!window.ShouldClose())
    {
        window.BeginDrawing();
        window.ClearBackground(raylib::BLACK);

        ScalingInfo scaling(
            window.GetWidth(),
            window.GetHeight(),
            mainConfig.getImgCanvasWidth(),
            mainConfig.getImgCanvasHeight()
        );

        HandleMenuInput(menu, window, mainConfig, scaling);

        // Draw base controller
        display.getTextures().unpressed.Draw(
            raylib::Vector2{ scaling.offsetX, scaling.offsetY },
            0.0f,
            scaling.scale,
            raylib::WHITE
        );

        // Handle gamepad input
        bool connected = display.updateGamepadConnection(raylib::Gamepad::IsAvailable(0));

        if (connected)
        {
            raylib::Gamepad gamepad(0);
            display.drawGamepadButtons(gamepad, scaling);
        }
        else
        {
            display.drawNoGamepadMessage(scaling);
        }

        if (menu.active != Menu::None)
        {
            DrawMenu(menu, scaling, mainConfig, 50, 50);
        }

        if (mainConfig.getCurrentWinHeight() != window.GetHeight() 
            || mainConfig.getCurrentWinWidth() != window.GetWidth())
        {
            mainConfig.updateWindowSize(window.GetWidth(), window.GetHeight());
        }

        window.EndDrawing();
    }

    if (mainConfig.getCurrentWinHeight() != mainConfig.getInitWinHeight()
        || mainConfig.getCurrentWinWidth() != mainConfig.getInitWinWidth())
    {
        mainConfig.updateInitWinSizes();
    }

    mainConfig.SaveConfig();

    return 0;
}