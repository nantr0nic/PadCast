#ifdef _WIN32
    #define NOGDI
    #define NOUSER
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif

#include "PadCast.h"
#include "Gamepad.hpp"

#ifdef _WIN32
    #undef NOGDI
    #undef NOUSER
#endif

#include <iostream>

GamepadTextures::GamepadTextures(Config::ControllerLayout layout)
: unpressed(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/controller.png"))
, pressedA(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/A.png"))
, pressedB(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/B.png"))
, pressedX(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/X.png"))
, pressedY(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/Y.png"))
, pressedUp(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/up.png"))
, pressedLeft(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/left.png"))
, pressedDown(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/down.png"))
, pressedRight(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/right.png"))
, pressedStart(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/start.png"))
, pressedSelect(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/select.png"))
, pressedLBump(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/L-bumper.png"))
, pressedRBump(PathManager::getResourcePath(Config::resourcesSubdir(layout) + "/images/pressed/R-bumper.png"))
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
    raylib::Color texture_tint;
    if (mConfig.getValue("Image", "USE_CUSTOM_TINT") == 1)
    {
        int r = mConfig.getValue("Image", "IMAGE_TINT_RED");
        int g = mConfig.getValue("Image", "IMAGE_TINT_GREEN");
        int b = mConfig.getValue("Image", "IMAGE_TINT_BLUE");
        texture_tint = Color{
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b),
            255
        };
    }
    else
    {
        int sel = mConfig.getValue("Image", "IMAGE_TINT_PALETTE");
        switch (sel)
        {
        case 1:  texture_tint = RED;    break;
        case 2:  texture_tint = GREEN;  break;
        case 3:  texture_tint = BLUE;   break;
        default: texture_tint = WHITE;  break;
        }
    }

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
    if (mConfig.getValue("Window", "USE_CUSTOM_BG") == 1)
    {
        int r = mConfig.getValue("Window", "CUSTOM_BG_RED");
        int g = mConfig.getValue("Window", "CUSTOM_BG_GREEN");
        int b = mConfig.getValue("Window", "CUSTOM_BG_BLUE");
        return Color{
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b),
            255
        };
    }

    int bgValue = mConfig.getBGColor();
    if (!isValidBackgroundColor(bgValue))
    {
        bgValue = 0;
    }

    switch (static_cast<BackgroundColor>(bgValue))
    {
    case BackgroundColor::Black:  return BLACK;
    case BackgroundColor::White:  return WHITE;
    case BackgroundColor::Red:    return RED;
    case BackgroundColor::Green:  return GREEN;
    case BackgroundColor::Blue:   return BLUE;
    default:                      return BLACK;
    }
}

void PadCast::loadButtonsFromConfig()
{
    mButtonMap.buttonIndex.clear();

    auto mapping = mConfig.loadButtonMapping(Config::ControllerLayout::SNES);

    if (mapping.empty())
    {
        mButtonMap.buttonIndex = mButtonMap.defaultSNESIndex;
        mButtonCache.refreshCache(mButtonMap);
        return;
    }

    for (const auto& [key, value] : mapping)
    {
        // Map INI keys to raylib button constants
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
