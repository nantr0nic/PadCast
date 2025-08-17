#include "PadCast.h"
#include "win_transparency.h"

#include <chrono>
#include <thread>
#include <print>
#include <iostream>


int main()
{
    bool debug_mode{ false };

    Config mainConfig{};

    // Window setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(
        mainConfig.getInitWinWidth(), 
        mainConfig.getInitWinHeight(),
        "PadCast");
    window.SetTargetFPS(mainConfig.getFPS());

    // Make window transparent if config file says so
    if (mainConfig.getTransparentWin() == 1)
    {
        SetWindowTransparency(GetWindowHandle());
    }

    GamepadDisplay display{ mainConfig };

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    SetTraceLogLevel(LOG_ALL);

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

        // Always draw base controller
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

           /* if (debug_mode)
            {
                int newButtonPress = gamepad.GetButtonPressed();
                if (newButtonPress > 0)
                {
                    TraceLog(LOG_DEBUG, "Button pressed: %d", newButtonPress);
                }
            }*/
        }
        else
        {
            display.drawNoGamepadMessage(scaling);
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
        TraceLog(LOG_INFO, "Updated INITIAL_WINDOW dimensions: %d x %d",
            window.GetWidth(), window.GetHeight());
    }

    mainConfig.SaveConfig();

    return 0;
}