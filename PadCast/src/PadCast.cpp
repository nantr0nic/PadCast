#include "PadCast.h"
#include "menus.h"

GamepadTextures::GamepadTextures()
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

ScalingInfo::ScalingInfo(int currentWidth, int currentHeight, 
                         int originalWidth, int originalHeight)
{
    float scaleX = static_cast<float>(currentWidth) / originalWidth;
    float scaleY = static_cast<float>(currentHeight) / originalHeight;
    scale = std::min(scaleX, scaleY);
    offsetX = (currentWidth - (originalWidth * scale)) / 2.0f;
    offsetY = (currentHeight - (originalHeight * scale)) / 2.0f;
}

void CachedButtons::refreshCache(const ButtonMap& buttonMap)
{
    //std::println("DEBUG: Starting refreshCache()");
    // reset each
    dpadUp = dpadRight = dpadDown = dpadLeft = 0;
    xButton = aButton = bButton = yButton = 0;
    leftTrigger = rightTrigger = selectButton = startButton = 0;
    for (const auto& [raylibButton, displayIndex] : buttonMap.buttonIndex)
    {
        //std::println("DEBUG: refreshing {} to {}", raylibButton, displayIndex);
        switch (raylibButton)
        {
        case 1: // D-pad UP display
            dpadUp = displayIndex;
            break;
        case 2: // D-pad RIGHT display
            dpadRight = displayIndex;
            break;
        case 3: // D-pad DOWN display
            dpadDown = displayIndex;
            break;
        case 4: // D-pad LEFT display
            dpadLeft = displayIndex;
            break;
        case 5: // X button display
            xButton = displayIndex;
            break;
        case 6: // A button display
            aButton = displayIndex;
            break;
        case 7: // B button display
            bButton = displayIndex;
            break;
        case 8: // Y button display
            yButton = displayIndex;
            break;
        case 9: // Left shoulder display
            leftTrigger = displayIndex;
            break;
        /* For future non-SNES controller use:
        case 10: // Left trigger display
            leftTrigger2 = displayIndex;
            */
        case 11: // Right shoulder display
            rightTrigger = displayIndex;
            break;
        /* For future non-SNES controller use:
        case 12: // Left trigger display
            leftTrigger2 = displayIndex;
            */
        case 13: // Select display
            selectButton = displayIndex;
            break;
        case 15: // Start display
            startButton = displayIndex;
            break;
        }
    }
}

//$ ----- PadCast ----- //

PadCast::PadCast(Config& mainConfig)
    : config{ mainConfig }
{
    if (config.getDebugMode() == 1)
    {
        debugMode = true;
    }

    loadButtonsFromConfig();
    buttonCache.refreshCache(buttonMap);
}

bool PadCast::updateGamepadConnection(bool currentlyAvailable)
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

void PadCast::drawGamepadButtons(const raylib::Gamepad& gamepad, 
                                 const ScalingInfo& scaling)
{
    const raylib::Vector2 position{ scaling.offsetX, scaling.offsetY };
    auto scale = scaling.scale;

    // pressed texture tint
    if (!tintCacheValid)
    {
        cachedUseCustomTint
            = config.getValue("Window", "USE_CUSTOM_TINT");
        cachedTintR = config.getValue("Window", "IMAGE_TINT_RED");
        cachedTintG = config.getValue("Window", "IMAGE_TINT_GREEN");
        cachedTintB = config.getValue("Window", "IMAGE_TINT_BLUE");
        if (cachedUseCustomTint == 1)
        {
            cachedTextureTint
                = Color{ (unsigned char)cachedTintR,
                         (unsigned char)cachedTintG,
                         (unsigned char)cachedTintB,
                         255 };
        }
        else
        {
            int sel = config.getValue("Window", "IMAGE_TINT_PALETTE");
            switch (sel)
            {
            case 1:
                cachedTextureTint = RED;
                break;
            case 2:
                cachedTextureTint = GREEN;
                break;
            case 3:
                cachedTextureTint = BLUE;
                break;
            default:
                cachedTextureTint = WHITE;
                break;
            }
        }
        tintCacheValid = true;
    }

    auto texture_tint = cachedTextureTint;

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
    if (gamepad.IsButtonDown(buttonCache.dpadUp))
    {
        textures.pressedUp.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(buttonCache.dpadRight))
    {
        textures.pressedRight.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(buttonCache.dpadDown))
    {
        textures.pressedDown.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(buttonCache.dpadLeft))
    {
        textures.pressedLeft.Draw(position, 0.0f, scale, texture_tint);
    }

    // Face buttons
    if (gamepad.IsButtonDown(buttonCache.xButton))
    {
        textures.pressedX.Draw(position, 0.0f, scale, texture_tint);
    }

    if (gamepad.IsButtonDown(buttonCache.aButton))
    {
        textures.pressedA.Draw(position, 0.0f, scale, texture_tint);
    }

    if (gamepad.IsButtonDown(buttonCache.bButton))
    {
        textures.pressedB.Draw(position, 0.0f, scale, texture_tint);
    }

    if (gamepad.IsButtonDown(buttonCache.yButton))
    {
        textures.pressedY.Draw(position, 0.0f, scale, texture_tint);
    }

    // Shoulder buttons
    if (gamepad.IsButtonDown(buttonCache.leftTrigger))
    {
        textures.pressedLBump.Draw(position, 0.0f, scale, texture_tint);
    }

    if (gamepad.IsButtonDown(buttonCache.rightTrigger))
    {
        textures.pressedRBump.Draw(position, 0.0f, scale, texture_tint);
    }

    // Select / Start
    if (gamepad.IsButtonDown(buttonCache.selectButton))
    {
        textures.pressedSelect.Draw(position, 0.0f, scale, texture_tint);
    }

    if (gamepad.IsButtonDown(buttonCache.startButton))
    {
        textures.pressedStart.Draw(position, 0.0f, scale, texture_tint);
    }
}

void PadCast::drawNoGamepadMessage(const ScalingInfo& scaling)
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

void PadCast::drawDebugButtonIndex(const raylib::Gamepad& gamepad,
                                   const ScalingInfo& scaling)
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

raylib::Color PadCast::getBGColor() const
{
    if (!configCacheValid)
    {
        // Read all config values once and cache them
        cachedUseCustomBG = config.getValue("Window", "USE_CUSTOM_BG");
        cachedCustomRed = config.getValue("Window", "CUSTOM_BG_RED");
        cachedCustomGreen = config.getValue("Window", "CUSTOM_BG_GREEN");
        cachedCustomBlue = config.getValue("Window", "CUSTOM_BG_BLUE");
        cachedBGColorValue = config.getBGColor();
        configCacheValid = true;
    }

    if (cachedUseCustomBG == 1)
    {
        if (cachedUseCustomBG != lastUseCustomBG ||
            cachedCustomRed != lastCustomRed ||
            cachedCustomGreen != lastCustomGreen ||
            cachedCustomBlue != lastCustomBlue)
        {
            lastUseCustomBG = cachedUseCustomBG;
            lastCustomRed = cachedCustomRed;
            lastCustomGreen = cachedCustomGreen;
            lastCustomBlue = cachedCustomBlue;

            cachedBGColor = Color{
                static_cast<unsigned char>(cachedCustomRed),
                static_cast<unsigned char>(cachedCustomGreen),
                static_cast<unsigned char>(cachedCustomBlue),
                255
            };
        }
    }
    else
    {
        // Palette mode
        if (cachedUseCustomBG != lastUseCustomBG || cachedBGColorValue != lastBGColorValue)
        {
            lastUseCustomBG = cachedUseCustomBG;
            lastBGColorValue = cachedBGColorValue;

            // Validate and update cached color
            int currentBGValue = cachedBGColorValue;
            if (!isValidBackgroundColor(currentBGValue))
            {
                currentBGValue = 0;
            }

            switch (static_cast<BackgroundColor>(currentBGValue))
            {
            case BackgroundColor::Black:
                cachedBGColor = BLACK;
                break;
            case BackgroundColor::White:
                cachedBGColor = WHITE;
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
    }

    return cachedBGColor;
}

void PadCast::loadButtonsFromConfig()
{
    //std::println("DEBUG: Checking if ButtonMap section exists...");
    // Clear the index
    buttonMap.buttonIndex.clear();
    //std::println("DEBUG: Cleared buttonIndex");

    if (!config.getIni().has("ButtonMap"))
    {
        //std::println("DEBUG: ButtonMap section NOT found, using default SNES map");
        buttonMap.buttonIndex = buttonMap.defaultSNESIndex;
        buttonCache.refreshCache(buttonMap);
        return;
    }

    //std::println("DEBUG: ButtonMap section found, loading mappings...");
    const auto& buttonSection = config.getIni().get("ButtonMap");

    //std::println("DEBUG: ButtonSection size: {}", buttonSection.size());

    for (const auto& [key, value_str] : buttonSection)
    {
        //std::println("DEBUG: Processing {} = {}", key, value_str);
        int value = 0;
        try {
            value = std::stoi(value_str);
            //std::println("DEBUG: Converted {} to int: {}", value_str, value);
        }
        catch (...) {
            std::println("DEBUG: Failed to convert {} to int", value_str);
            continue;
        }

        // Map INI keys to raylib button constants
        bool mapped = false;
        if (key == "DPAD_UP") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_UP] = value;
        }
        else if (key == "DPAD_RIGHT") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = value;
        }
        else if (key == "DPAD_DOWN") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_DOWN] = value;
        }
        else if (key == "DPAD_LEFT") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_LEFT] = value;
        }
        else if (key == "X_BUTTON") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_UP] = value;
        }
        else if (key == "A_BUTTON") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = value;
        }
        else if (key == "B_BUTTON") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_DOWN] = value;
        }
        else if (key == "Y_BUTTON") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_LEFT] = value;
        }
        else if (key == "L_BUTTON") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_TRIGGER_1] = value;
        }
        else if (key == "R_BUTTON") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = value;
        }
        else if (key == "SELECT") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_MIDDLE_LEFT] = value;
        }
        else if (key == "START") {
            buttonMap.buttonIndex[GAMEPAD_BUTTON_MIDDLE_RIGHT] = value;
        }
    }

    //std::println("DEBUG: Final buttonIndex size: {}", buttonMap.buttonIndex.size());

    // Safety check - if somehow buttonIndex is still empty, use defaults
    if (buttonMap.buttonIndex.empty())
    {
        std::println("DEBUG: WARNING - buttonIndex still empty, using defaults");
        buttonMap.buttonIndex = buttonMap.defaultSNESIndex;
    }

    // Refresh the cache after loading
    buttonCache.refreshCache(buttonMap);
    //std::println("DEBUG: Finished loading button mappings");
}