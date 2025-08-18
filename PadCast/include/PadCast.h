#ifndef PADCAST_H
#define PADCAST_H

#include <raylib.h>
#include <raylib-cpp.hpp>
#include "config.h"

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

    GamepadTextures()
        : unpressed("resources/images/controller.png")
        , pressedA("resources/images/pressed/A.png")
        , pressedB("resources/images/pressed/B.png")
        , pressedX("resources/images/pressed/X.png")
        , pressedY("resources/images/pressed/Y.png")
        , pressedUp("resources/images/pressed/up.png")
        , pressedLeft("resources/images/pressed/left.png")
        , pressedDown("resources/images/pressed/down.png")
        , pressedRight("resources/images/pressed/right.png")
        , pressedStart("resources/images/pressed/start.png")
        , pressedSelect("resources/images/pressed/select.png")
        , pressedLBump("resources/images/pressed/L-bumper.png")
        , pressedRBump("resources/images/pressed/R-bumper.png")
    {
    }
};

struct ScalingInfo
{
    float scale{};
    float offsetX{};
    float offsetY{};

    ScalingInfo(int currentWidth, int currentHeight, int originalWidth, int originalHeight)
    {
        float scaleX = static_cast<float>(currentWidth) / originalWidth;
        float scaleY = static_cast<float>(currentHeight) / originalHeight;
        scale = std::min(scaleX, scaleY);
        offsetX = (currentWidth - (originalWidth * scale)) / 2.0f;
        offsetY = (currentHeight - (originalHeight * scale)) / 2.0f;
    }
};

class GamepadDisplay
{
private:
    GamepadTextures textures;
    bool gamepadWasConnected{ false };
    int stabilityCounter{ 0 };
    Config& config;
    bool debugMode{ false };

public:
    GamepadDisplay(Config& mainConfig)
        : config{ mainConfig }
    {
        if (config.getDebugMode() == 1)
        {
            debugMode = true;
        }
    }

    const GamepadTextures& getTextures() const { return textures; }
    Config& getConfig() { return config; }

public:
    // Gamepad Display functions
    bool updateGamepadConnection(bool currentlyAvailable)
    {
        if (currentlyAvailable == gamepadWasConnected)
        {
            stabilityCounter = 0;
        }
        else
        {
            ++stabilityCounter;
            if (stabilityCounter >= config.getValue("Gamepad", "STABILITY_THRESHOLD"))
            {
                gamepadWasConnected = currentlyAvailable;
                stabilityCounter = 0;
            }
        }
        return gamepadWasConnected;
    }

    void drawGamepadButtons(const raylib::Gamepad& gamepad, const ScalingInfo& scaling)
    {
        const raylib::Vector2 position{ scaling.offsetX, scaling.offsetY };

        if (debugMode)
        {
            int newButtonPress = gamepad.GetButtonPressed();
            if (newButtonPress > 0)
            {
                TraceLog(LOG_DEBUG, "Button pressed: %d", newButtonPress);
                drawDebugButtonIndex(gamepad, scaling);
            }

            for (int axisIndex = 0; axisIndex < 6; ++axisIndex)  // Standard gamepads have 6 axes
            {
                float axisValue = gamepad.GetAxisMovement(axisIndex);
                if (std::abs(axisValue) > 0.2f)  // Use dead zone to filter noise
                {
                    TraceLog(LOG_INFO, "Axis %d movement: %.3f", axisIndex, axisValue);
                }
            }

            // N64 controller:
            //bool debugN64{ false };
            //if (debugN64)
            //{
            //    // The joystick
            //    // GAMEPAD_AXIS_LEFT_X
            //    float axisZero = gamepad.GetAxisMovement(0);
            //    // GAMEPAD_AXIS_LEFT_Y
            //    float axisOne = gamepad.GetAxisMovement(1);
            //    // The Z button
            //    // -1 is unpressed, 1 is pressed
            //    // or GAMEPAD_AXIS_LEFT_TRIGGER
            //    float buttonZ = gamepad.GetAxisMovement(4);
            //}
        }
      
        // This had been tested with SNES
        // Face buttons (A, B, X, Y)
        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(7)))
        {
            textures.pressedB.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(6)))
        {
            textures.pressedA.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_RIGHT_FACE_UP) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(5)))
        {
            textures.pressedX.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(8)))
        {
            textures.pressedY.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        // D-pad buttons
        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_LEFT_FACE_UP) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(1)))
        {
            textures.pressedUp.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(4)))
        {
            textures.pressedLeft.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(3)))
        {
            textures.pressedDown.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(2)))
        {
            textures.pressedRight.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        // Start
        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(15)))
        {
            textures.pressedStart.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }
        // Select
        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_MIDDLE_LEFT) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(13)))
        {
            textures.pressedSelect.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        // Shoulder buttons
        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(9)))
        {
            textures.pressedLBump.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }
        
        if (gamepad.IsButtonDown(GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
            gamepad.IsButtonDown(static_cast<GamepadButton>(11)))
        {
            textures.pressedRBump.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }
    }

    void drawNoGamepadMessage(const ScalingInfo& scaling)
    {
        int fontSize = std::max(
            static_cast<int>(config.getValue("Font", "DEFAULT_FONT_SIZE") * scaling.scale),
            config.getValue("Font", "MIN_FONT_SIZE")
        );

        raylib::DrawText(
            "No Gamepad Connected",
            static_cast<int>(config.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetX),
            static_cast<int>(config.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetY),
            fontSize,
            raylib::Color(raylib::WHITE)
        );
    }

    void drawDebugButtonIndex(const raylib::Gamepad& gamepad, const ScalingInfo& scaling)
    // if debug mode is set to 1 in config.ini, this will print the button index in the window
    {
        int fontSize = std::max(
            static_cast<int>(config.getValue("Font", "DEFAULT_FONT_SIZE") * scaling.scale),
            config.getValue("Font", "MIN_FONT_SIZE")
        );

        int buttonIndex = gamepad.GetButtonPressed();
        std::string buttonPressed = std::to_string(buttonIndex);

        raylib::DrawText(
            buttonPressed,
            static_cast<int>(config.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetX),
            static_cast<int>(config.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetY),
            fontSize,
            raylib::Color(raylib::WHITE)
        );
    }
};

#endif