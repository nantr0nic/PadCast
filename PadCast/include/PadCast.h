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

enum class BackgroundColor
{
    Black,      // 0
    White,      // 1
    Raywhite,   // 2
    Red,        // 3
    Green,      // 4
    Blue        // 5
};

struct ButtonMap
{
    std::unordered_map<int, int> buttonIndex;
    std::unordered_map<int, int> defaultSNESIndex{
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

    bool configNeedsUpdate{ false };

    ButtonMap()
    {
        // For now, default to SNES index
        buttonIndex = defaultSNESIndex;
    }

    void remapButton(int raylibButton, int newIndex)
    {
        // save for remap
        buttonIndex[raylibButton] = newIndex;
    }
};

struct CachedButtons
{
    // Cache all frequently used button indices
    int dpadUp;
    int dpadRight;
    int dpadDown;
    int dpadLeft;
    int xButton;        // RIGHT_FACE_UP
    int aButton;        // RIGHT_FACE_RIGHT
    int bButton;        // RIGHT_FACE_DOWN
    int yButton;        // RIGHT_FACE_LEFT
    int leftTrigger;
    int rightTrigger;
    int selectButton;   // MIDDLE_LEFT
    int startButton;    // MIDDLE_RIGHT

    CachedButtons(const ButtonMap& buttonMap)
    {
        refreshCache(buttonMap);
    }

    void refreshCache(const ButtonMap& buttonMap)
    {
        // Cache all button indices for fast access
        dpadUp = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_LEFT_FACE_UP);
        dpadRight = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
        dpadDown = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_LEFT_FACE_DOWN);
        dpadLeft = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_LEFT_FACE_LEFT);
        xButton = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_RIGHT_FACE_UP);
        aButton = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
        bButton = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        yButton = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
        leftTrigger = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_LEFT_TRIGGER_1);
        rightTrigger = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_RIGHT_TRIGGER_1);
        selectButton = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_MIDDLE_LEFT);
        startButton = buttonMap.buttonIndex.at(GAMEPAD_BUTTON_MIDDLE_RIGHT);
    }
};

class GamepadDisplay
{
private:
    GamepadTextures textures;
    Config& config;
    ButtonMap buttonMap;
    CachedButtons buttons;

    bool gamepadWasConnected{ false };
    int stabilityCounter{ 0 };

    // Cache values for optimizing calls
    mutable Color cachedBGColor{ BLACK };
    mutable int lastBGColorValue{ -1 };
    mutable int cachedStabilityThreshold{ -1 };

    bool debugMode{ false };

public:
    GamepadDisplay(Config& mainConfig)
        : config{ mainConfig }
        , buttons{ buttonMap }
    {
        if (config.getDebugMode() == 1)
        {
            debugMode = true;
        }
    }

    const GamepadTextures& getTextures() const { return textures; }
    Config& getConfig() { return config; }
    bool isDebugOn() const { return debugMode; }

public:
    // Gamepad Display functions
    bool updateGamepadConnection(bool currentlyAvailable)
    {
        // Cache stability threshold
        if (cachedStabilityThreshold == -1)
        {
            cachedStabilityThreshold = config.getValue("Gamepad", "STABILITY_THRESHOLD");
        }

        if (currentlyAvailable == gamepadWasConnected)
        {
            stabilityCounter = 0;
        }
        else
        {
            ++stabilityCounter;
            if (stabilityCounter >= cachedStabilityThreshold)
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
      
        // D-Pad
        if (gamepad.IsButtonDown(buttons.dpadUp))
        {
            textures.pressedUp.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }
        if (gamepad.IsButtonDown(buttons.dpadRight))
        {
            textures.pressedRight.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }
        if (gamepad.IsButtonDown(buttons.dpadDown))
        {
            textures.pressedDown.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }
        if (gamepad.IsButtonDown(buttons.dpadLeft))
        {
            textures.pressedLeft.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        // Face buttons
        if (gamepad.IsButtonDown(buttons.xButton))
        {
            textures.pressedX.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(buttons.aButton))
        {
            textures.pressedA.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(buttons.bButton))
        {
            textures.pressedB.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(buttons.yButton))
        {
            textures.pressedY.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        // Shoulder buttons
        if (gamepad.IsButtonDown(buttons.leftTrigger))
        {
            textures.pressedLBump.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(buttons.rightTrigger))
        {
            textures.pressedRBump.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        // Select / Start
        if (gamepad.IsButtonDown(buttons.selectButton))
        {
            textures.pressedSelect.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
        }

        if (gamepad.IsButtonDown(buttons.startButton))
        {
            textures.pressedStart.Draw(position, 0.0f, scaling.scale, raylib::WHITE);
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

public:
    // Other functions (display, etc.)
    bool isValidBackgroundColor(int value) const
    {
        return (value >= 0 && value <= static_cast<int>(BackgroundColor::Blue));
    }
    Color getBGColor() const
    {
        int currentBGValue = config.getBGColor();

        // Only change if config value changed
        if (currentBGValue != lastBGColorValue)
        { 
            lastBGColorValue = currentBGValue;

            if (!isValidBackgroundColor(currentBGValue))
            {
                currentBGValue = 0; // Default to black
            }

            BackgroundColor bgColor = static_cast<BackgroundColor>(currentBGValue);
            switch (bgColor)
            {
            case BackgroundColor::Black:
                cachedBGColor = BLACK;
                break;
            case BackgroundColor::White:
                cachedBGColor = WHITE;
                break;
            case BackgroundColor::Raywhite:
                cachedBGColor = RAYWHITE;
                break;
            case BackgroundColor::Red:
                cachedBGColor = RED;
                break;
            case BackgroundColor::Green:
                cachedBGColor = GREEN;
                break;
            case BackgroundColor::Blue:
                cachedBGColor = BLUE;
                break;
            default:
                cachedBGColor = BLACK;
                break;
            }
        }
        return cachedBGColor;
    }

    void resetButtonsToDefault()
    {
        buttonMap.buttonIndex = buttonMap.defaultSNESIndex;
        buttons.refreshCache(buttonMap);
    }
    void refreshButtonCache() { buttons.refreshCache(buttonMap); }
    void setButtonMap(int raylibButton, int newIndex) 
    { 
        buttonMap.remapButton(raylibButton, newIndex);
        buttons.refreshCache(buttonMap);
    }
};

#endif