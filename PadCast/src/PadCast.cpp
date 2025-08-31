#include "PadCast.h"
#include "Gamepad.hpp"
#include <iostream>

GamepadTextures::GamepadTextures()
: unpressed(PathManager::getResourcePath("images/controller.png"))
, pressedA(PathManager::getResourcePath("images/pressed/A.png"))
, pressedB(PathManager::getResourcePath("images/pressed/B.png"))
, pressedX(PathManager::getResourcePath("images/pressed/X.png"))
, pressedY(PathManager::getResourcePath("images/pressed/Y.png"))
, pressedUp(PathManager::getResourcePath("images/pressed/up.png"))
, pressedLeft(PathManager::getResourcePath("images/pressed/left.png"))
, pressedDown(PathManager::getResourcePath("images/pressed/down.png"))
, pressedRight(PathManager::getResourcePath("images/pressed/right.png"))
, pressedStart(PathManager::getResourcePath("images/pressed/start.png"))
, pressedSelect(PathManager::getResourcePath("images/pressed/select.png"))
, pressedLBump(PathManager::getResourcePath("images/pressed/L-bumper.png"))
, pressedRBump(PathManager::getResourcePath("images/pressed/R-bumper.png"))
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
    //std::cout << "DEBUG: Starting refreshCache()" << std::endl;
    // reset each
    dpadUp = dpadRight = dpadDown = dpadLeft = 0;
    xButton = aButton = bButton = yButton = 0;
    leftTrigger = rightTrigger = selectButton = startButton = 0;
    for (const auto& [raylibButton, displayIndex] : buttonMap.buttonIndex)
    {
        //std::cout << "DEBUG: refreshing " << raylibButton << " to " << displayIndex << std::endl;
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
            break;
            */
        case 11: // Right shoulder display
            rightTrigger = displayIndex;
            break;
        /* For future non-SNES controller use:
        case 12: // Left trigger display
            leftTrigger2 = displayIndex;
            break;
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
    : mConfig{ mainConfig }
{
    if (mConfig.getDebugMode() == 1)
    {
        mDebugMode = true;
    }

    gamepadIndex = mainConfig.getGPIndex();

    loadButtonsFromConfig();
    mButtonCache.refreshCache(mButtonMap);
}

bool PadCast::updateGamepadConnection(bool currentlyAvailable)
{
    // Cache stability threshold
    if (mCachedStabilityThreshold == -1)
    {
        mCachedStabilityThreshold = mConfig.getValue("Gamepad", "STABILITY_THRESHOLD");
    }

    if (currentlyAvailable == mGamepadWasConnected)
    {
        mStabilityCounter = 0;
    }
    else
    {
        ++mStabilityCounter;
        if (mStabilityCounter >= mCachedStabilityThreshold)
        {
            mGamepadWasConnected = currentlyAvailable;
            mStabilityCounter = 0;
        }
    }
    return mGamepadWasConnected;
}

void PadCast::drawGamepadButtons(const raylib::Gamepad& gamepad, 
                                 const ScalingInfo& scaling)
{
    const raylib::Vector2 position{ scaling.offsetX, scaling.offsetY };
    auto scale = scaling.scale;

    // pressed texture tint
    if (!mTintCacheValid)
    {
        mCachedUseCustomTint
            = mConfig.getValue("Image", "USE_CUSTOM_TINT");
        mCachedTintR = mConfig.getValue("Image", "IMAGE_TINT_RED");
        mCachedTintG = mConfig.getValue("Image", "IMAGE_TINT_GREEN");
        mCachedTintB = mConfig.getValue("Image", "IMAGE_TINT_BLUE");
        if (mCachedUseCustomTint == 1)
        {
            mCachedPressedTint = Color { 
                (unsigned char)mCachedTintR,
                (unsigned char)mCachedTintG,
                (unsigned char)mCachedTintB,
                255
            };
        }
        else
        {
            int sel = mConfig.getValue("Image", "IMAGE_TINT_PALETTE");
            switch (sel)
            {
            case 1:
                mCachedPressedTint = RED;
                break;
            case 2:
                mCachedPressedTint = GREEN;
                break;
            case 3:
                mCachedPressedTint = BLUE;
                break;
            default:
                mCachedPressedTint = WHITE;
                break;
            }
        }
        mTintCacheValid = true;
    }

    auto texture_tint = mCachedPressedTint;

    if (mDebugMode)
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
    if (gamepad.IsButtonDown(mButtonCache.dpadUp))
    {
        mTextures.pressedUp.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.dpadRight))
    {
        mTextures.pressedRight.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.dpadDown))
    {
        mTextures.pressedDown.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.dpadLeft))
    {
        mTextures.pressedLeft.Draw(position, 0.0f, scale, texture_tint);
    }

    // Face buttons
    if (gamepad.IsButtonDown(mButtonCache.xButton))
    {
        mTextures.pressedX.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.aButton))
    {
        mTextures.pressedA.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.bButton))
    {
        mTextures.pressedB.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.yButton))
    {
        mTextures.pressedY.Draw(position, 0.0f, scale, texture_tint);
    }

    // Shoulder buttons
    if (gamepad.IsButtonDown(mButtonCache.leftTrigger))
    {
        mTextures.pressedLBump.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.rightTrigger))
    {
        mTextures.pressedRBump.Draw(position, 0.0f, scale, texture_tint);
    }

    // Select / Start
    if (gamepad.IsButtonDown(mButtonCache.selectButton))
    {
        mTextures.pressedSelect.Draw(position, 0.0f, scale, texture_tint);
    }
    if (gamepad.IsButtonDown(mButtonCache.startButton))
    {
        mTextures.pressedStart.Draw(position, 0.0f, scale, texture_tint);
    }
}

void PadCast::drawNoGamepadMessage(const ScalingInfo& scaling)
{
    int fontSize = std::max(
        static_cast<int>(mConfig.getValue("Font", "DEFAULT_FONT_SIZE") * scaling.scale),
        mConfig.getValue("Font", "MIN_FONT_SIZE")
    );

    raylib::DrawText(
        "No Gamepad Connected",
        static_cast<int>(mConfig.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetX),
        static_cast<int>(mConfig.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetY),
        fontSize,
        raylib::Color(raylib::WHITE)
    );
}

void PadCast::findGamepadIndex()
{
    // For debugging...
    // I'm unsure if this needs to be more than 3 (zero-index), 
    // it seems GLFW doesn't go past 4 devices...
    // So if it is registering keyboard and mouse and a bluetooth device, it will
    // only detect 1 of 2 plugged in controllers (by default?)
    for (int i = 0; i < 4; ++i)
    {
        if (raylib::Gamepad::IsAvailable(i))
        {
            raylib::Gamepad tempGP(i);
            // testing this
            std::cout << "Gamepad found: " << tempGP.GetName() << " at " << tempGP.GetNumber() << std::endl;
        }
    }
}

void PadCast::drawDebugButtonIndex(const raylib::Gamepad& gamepad,
                                   const ScalingInfo& scaling)
// if debug mode is set to 1 in config.ini, this will print the button index in the window
{
    int fontSize = std::max(
        static_cast<int>(mConfig.getValue("Font", "DEFAULT_FONT_SIZE") * scaling.scale),
        mConfig.getValue("Font", "MIN_FONT_SIZE")
    );

    int buttonIndex = gamepad.GetButtonPressed();
    std::string buttonPressed = std::to_string(buttonIndex);

    raylib::DrawText(
        buttonPressed,
        static_cast<int>(mConfig.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetX),
        static_cast<int>(mConfig.getValue("Font", "TEXT_OFFSET") * scaling.scale + scaling.offsetY),
        fontSize,
        raylib::Color(raylib::WHITE));
}

void PadCast::debugGamepadInfo(const raylib::Gamepad &gamepad)
// this will print what gamepads are available to the log window
// good for troubleshooting devices on linux
{
  std::cout << "=== Gamepad Debug Info ===" << std::endl;

  for (int i = 0; i < 4; ++i) {
    bool available = raylib::Gamepad::IsAvailable(i);
    std::cout << "Gamepad " << i << ": Available = " << available << std::endl;

    if (available) 
    {
        raylib::Gamepad testPad(i);
        std::cout << "  Name: " << testPad.GetName() << std::endl;

        // Button test -- press a button on the controller and see if its #0-4
        for (int btn = 0; btn < 16; ++btn) 
        {
            if (testPad.IsButtonDown(btn)) 
            {
            std::cout << "  Button " << btn << " pressed on gamepad " << i << std::endl;
            }
        }
    }
  }
  std::cout << "========================" << std::endl;
}

raylib::Color PadCast::getBGColor() const
{
    if (!mBGCacheValid)
    {
        // Read all config values once and cache them
        mCachedUseCustomBG = mConfig.getValue("Window", "USE_CUSTOM_BG");
        mCachedCustomRed = mConfig.getValue("Window", "CUSTOM_BG_RED");
        mCachedCustomGreen = mConfig.getValue("Window", "CUSTOM_BG_GREEN");
        mCachedCustomBlue = mConfig.getValue("Window", "CUSTOM_BG_BLUE");
        mCachedBGColorValue = mConfig.getBGColor();
        mBGCacheValid = true;
    }

    if (mCachedUseCustomBG == 1)
    {
        if (mCachedUseCustomBG != mLastUseCustomBG ||
            mCachedCustomRed != mLastCustomRed ||
            mCachedCustomGreen != mLastCustomGreen ||
            mCachedCustomBlue != mLastCustomBlue)
        {
            mLastUseCustomBG = mCachedUseCustomBG;
            mLastCustomRed = mCachedCustomRed;
            mLastCustomGreen = mCachedCustomGreen;
            mLastCustomBlue = mCachedCustomBlue;

            mCachedBGColor = Color{
                static_cast<unsigned char>(mCachedCustomRed),
                static_cast<unsigned char>(mCachedCustomGreen),
                static_cast<unsigned char>(mCachedCustomBlue),
                255
            };
        }
    }
    else
    {
        // Palette mode
        if (mCachedUseCustomBG != mLastUseCustomBG || mCachedBGColorValue != mLastBGColorValue)
        {
            mLastUseCustomBG = mCachedUseCustomBG;
            mLastBGColorValue = mCachedBGColorValue;

            // Validate and update cached color
            int currentBGValue = mCachedBGColorValue;
            if (!isValidBackgroundColor(currentBGValue))
            {
                currentBGValue = 0;
            }

            switch (static_cast<BackgroundColor>(currentBGValue))
            {
            case BackgroundColor::Black:
                mCachedBGColor = BLACK;
                break;
            case BackgroundColor::White:
                mCachedBGColor = WHITE;
                break;
            case BackgroundColor::Red:
                mCachedBGColor = RED;
                break;
            case BackgroundColor::Green:
                mCachedBGColor = GREEN;
                break;
            case BackgroundColor::Blue:
                mCachedBGColor = BLUE;
                break;
            default:
                mCachedBGColor = BLACK;
                break;
            }
        }
    }

    return mCachedBGColor;
}

void PadCast::loadButtonsFromConfig()
{
    //std::cout << "DEBUG: Checking if ButtonMap section exists..." << std::endl;
    // Clear the index
    mButtonMap.buttonIndex.clear();
    //std::cout << "DEBUG: Cleared buttonIndex" << std::endl;

    if (!mConfig.getIni().has("ButtonMap"))
    {
        //std::cout << "DEBUG: ButtonMap section NOT found, using default SNES map" << std::endl;
        mButtonMap.buttonIndex = mButtonMap.defaultSNESIndex;
        mButtonCache.refreshCache(mButtonMap);
        return;
    }

    //std::cout << "DEBUG: ButtonMap section found, loading mappings..." << std::endl;
    const auto& buttonSection = mConfig.getIni().get("ButtonMap");
    //std::cout << "DEBUG: ButtonSection size: " << buttonSection.size() << std::endl;

    for (const auto& [key, value_str] : buttonSection)
    {
        //std::cout << "DEBUG: Processing " << key << " = " << value_str << std::endl;
        int value = 0;
        try {
            value = std::stoi(value_str);
            //std::cout << "DEBUG: Converted " << value_str << " to int: " << value << std::endl;
        }
        catch (...) {
            std::cerr << "DEBUG: Failed to convert " << value_str << " to int" << std::endl;
            continue;
        }

        // Map INI keys to raylib button constants
        bool mapped = false;
        if (key == "DPAD_UP") 
        {
            mButtonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_UP] = value;
        }
        else if (key == "DPAD_RIGHT") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = value;
        }
        else if (key == "DPAD_DOWN") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_DOWN] = value;
        }
        else if (key == "DPAD_LEFT") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_FACE_LEFT] = value;
        }
        else if (key == "X_BUTTON") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_UP] = value;
        }
        else if (key == "A_BUTTON") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = value;
        }
        else if (key == "B_BUTTON") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_DOWN] = value;
        }
        else if (key == "Y_BUTTON") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_FACE_LEFT] = value;
        }
        else if (key == "L_BUTTON") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_LEFT_TRIGGER_1] = value;
        }
        else if (key == "R_BUTTON") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = value;
        }
        else if (key == "SELECT") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_MIDDLE_LEFT] = value;
        }
        else if (key == "START") 
        {
                mButtonMap.buttonIndex[GAMEPAD_BUTTON_MIDDLE_RIGHT] = value;
        }
    }

    //std::cout << "DEBUG: Final buttonIndex size: " << mButtonMap.buttonIndex.size() << std::endl;

    // Safety check - if somehow buttonIndex is still empty, use defaults
    if (mButtonMap.buttonIndex.empty())
    {
        std::cerr << "DEBUG: WARNING - buttonIndex still empty, using defaults" << std::endl;
        mButtonMap.buttonIndex = mButtonMap.defaultSNESIndex;
    }

    // Refresh the cache after loading
    mButtonCache.refreshCache(mButtonMap);
    // std::cout << "DEBUG: Finished loading button mappings" << std::endl;
}
