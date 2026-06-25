#include "menus.h"
#include "PadCast.h"
#include "config.h"
#include "debounce.h"

#include <string>

namespace {

    // Remap state — extracted from static locals in RemapButtonScreens().
    // Lifetime is program duration (same as the old function-local statics).
    struct RemapState
    {
    	bool isRemapping = false;
    	bool waitingForInput = false;
    	int buttonPromptIndex = 0;
    	raylib::Gamepad gamepad{ 0 };
    	DebounceTimer buttonDebounce{ 0.5f };
    	float lastAttemptTime = 0.0f;

    	void init()
    	{
    		isRemapping = true;
    		waitingForInput = true;
    		buttonPromptIndex = 0;
    		buttonDebounce.Reset();
    		lastAttemptTime = 0.0f;
    	}

    	void cleanup()
    	{
    		isRemapping = false;
    		waitingForInput = false;
    		buttonPromptIndex = 0;
    	}
    };

RemapState gRemapState;

struct JoystickRemapState
{
	int stepIdx = 0;
	bool active = false;
	bool waitingCenter = false;
	DebounceTimer debounce{ 1.0f };
	float axisBaseline[6]{};

	void init()
	{
		active = true;
		stepIdx = 0;
		waitingCenter = true;
		debounce.Reset();
	}

	void cleanup()
	{
		stepIdx = 0;
		active = false;
		waitingCenter = false;
		debounce.Reset();
	}
};

JoystickRemapState gJoystickRemapState;

    // Remap step data — per-layout button definitions for the remap walkthrough.
    struct RemapStep
    {
    	const char* prompt;
    	int raylibButton;  // -1 if analog
    	int axisIndex;     // -1 if digital, -2 = stick scan, >=0 = specific axis
    	int stickDir;      // -1 = not a stick step; 0=Up,1=Right,2=Down,3=Left
    	Config::ButtonConfigKey configKey;
    };

    // SNES: 12 digital buttons
    const RemapStep kSnesRemapSteps[]{
    	{ "Press D-pad UP",      GAMEPAD_BUTTON_LEFT_FACE_UP,    -1,-1, Config::ButtonConfigKey::DPAD_UP },
    	{ "Press D-pad RIGHT",   GAMEPAD_BUTTON_LEFT_FACE_RIGHT, -1,-1, Config::ButtonConfigKey::DPAD_RIGHT },
    	{ "Press D-pad DOWN",    GAMEPAD_BUTTON_LEFT_FACE_DOWN,  -1,-1, Config::ButtonConfigKey::DPAD_DOWN },
    	{ "Press D-pad LEFT",    GAMEPAD_BUTTON_LEFT_FACE_LEFT,  -1,-1, Config::ButtonConfigKey::DPAD_LEFT },
    	{ "Press X",             GAMEPAD_BUTTON_RIGHT_FACE_UP,   -1,-1, Config::ButtonConfigKey::X_BUTTON },
    	{ "Press A",             GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,-1,-1, Config::ButtonConfigKey::A_BUTTON },
    	{ "Press B",             GAMEPAD_BUTTON_RIGHT_FACE_DOWN, -1,-1, Config::ButtonConfigKey::B_BUTTON },
    	{ "Press Y",             GAMEPAD_BUTTON_RIGHT_FACE_LEFT, -1,-1, Config::ButtonConfigKey::Y_BUTTON },
    	{ "Press LEFT Shoulder", GAMEPAD_BUTTON_LEFT_TRIGGER_1,  -1,-1, Config::ButtonConfigKey::L_BUTTON },
    	{ "Press RIGHT Shoulder",GAMEPAD_BUTTON_RIGHT_TRIGGER_1, -1,-1, Config::ButtonConfigKey::R_BUTTON },
    	{ "Press Select",        GAMEPAD_BUTTON_MIDDLE_LEFT,     -1,-1, Config::ButtonConfigKey::SELECT },
    	{ "Press Start",         GAMEPAD_BUTTON_MIDDLE_RIGHT,    -1,-1, Config::ButtonConfigKey::START },
    };
    constexpr int kSnesStepCount = sizeof(kSnesRemapSteps) / sizeof(kSnesRemapSteps[0]);

    // N64: 18 steps (13 digital + Z trigger + 4 joystick directions)
    const RemapStep kN64RemapSteps[]{
    	{ "Press D-pad UP",      GAMEPAD_BUTTON_LEFT_FACE_UP,    -1,-1, Config::ButtonConfigKey::DPAD_UP },
    	{ "Press D-pad RIGHT",   GAMEPAD_BUTTON_LEFT_FACE_RIGHT, -1,-1, Config::ButtonConfigKey::DPAD_RIGHT },
    	{ "Press D-pad DOWN",    GAMEPAD_BUTTON_LEFT_FACE_DOWN,  -1,-1, Config::ButtonConfigKey::DPAD_DOWN },
    	{ "Press D-pad LEFT",    GAMEPAD_BUTTON_LEFT_FACE_LEFT,  -1,-1, Config::ButtonConfigKey::DPAD_LEFT },
    	{ "Press A",             GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,-1,-1, Config::ButtonConfigKey::A_BUTTON },
    	{ "Press B",             GAMEPAD_BUTTON_RIGHT_FACE_DOWN, -1,-1, Config::ButtonConfigKey::B_BUTTON },
    	{ "Press C-Up",          20,                      -1,-1, Config::ButtonConfigKey::C_UP },
    	{ "Press C-Right",       21,                      -1,-1, Config::ButtonConfigKey::C_RIGHT },
    	{ "Press C-Down",        22,                       -1,-1, Config::ButtonConfigKey::C_DOWN },
    	{ "Press C-Left",        23,                       -1,-1, Config::ButtonConfigKey::C_LEFT },
    	{ "Press L",             GAMEPAD_BUTTON_LEFT_TRIGGER_1,  -1,-1, Config::ButtonConfigKey::L_BUTTON },
    	{ "Press R",             GAMEPAD_BUTTON_RIGHT_TRIGGER_1, -1,-1, Config::ButtonConfigKey::R_BUTTON },
    	{ "Press Start",         GAMEPAD_BUTTON_MIDDLE_RIGHT,    -1,-1, Config::ButtonConfigKey::START },
    	{ "Press Z Trigger",     -1,                      4, -1, Config::ButtonConfigKey::Z_BUTTON },
    };
    constexpr int kN64StepCount = sizeof(kN64RemapSteps) / sizeof(kN64RemapSteps[0]);

}

MenuItem createMenuItem(const std::string& label, std::function<void()> action)
{
	return { label, action };
}

MenuItem createBackMenuItem(MenuContext::MenuParams& params)
{
	return createMenuItem("Back", [&params]() {
		params.menu.active = Menu::Main;
		SetupMainMenu(params);
		});
}

MenuItem createCloseMenuItem(MenuContext& menu)
{
	return createMenuItem("Close", [&menu]() {
		menu.active = Menu::None;
		});
}

MenuItem createSpacer()
{
	return createMenuItem(" ", []() {});	// lol C++
}

void SetupMainMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"Video",
		[&params]() {
			params.menu.active = Menu::Video;
			SetupVideoMenu(params);
		}
		});
	params.menu.items.push_back({
		"Visuals",
		[&params]() {
			params.menu.active = Menu::Visuals;
			SetupVisualsMenu(params);
		}
		});
	params.menu.items.push_back({
		"Controller",
		[&params]() {
			params.menu.active = Menu::Controller;
			SetupControllerMenu(params);
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back({
		"Reload Config File",
		[&params]() {
			params.config.reloadConfig();
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void SetupVideoMenu(MenuContext::MenuParams& params)
{
	std::string current_vsync = (std::to_string(params.config.getValue("Window", "USE_VSYNC")) == "1") ? "On" : "Off";
	std::string vsync_string = "Toggle VSync \n(Currently: " + current_vsync + ")";
	params.menu.items.clear();
	params.menu.items.push_back({
		"Resolution",
		[&params]() {
			params.menu.active = Menu::Resolution;
			SetupResolutionMenu(params);
		}
		});
	params.menu.items.push_back({
		"Target FPS",
		[&params]() {
			params.menu.active = Menu::FPS;
			SetupFPSMenu(params);
		}
		});
	params.menu.items.push_back({
		vsync_string,
		[&params]() {
			bool vsync = params.config.getVSYNC();
			if (vsync)
			{
				ClearWindowState(FLAG_VSYNC_HINT);
				params.window.SetTargetFPS(params.config.getValue("Window", "TARGET_FPS"));
				params.config.updateUseVSYNC(0);
				SetupVideoMenu(params);
			}
			else
			{
				int currentMonitor = GetCurrentMonitor();
				int refreshRate = GetMonitorRefreshRate(currentMonitor);
				SetWindowState(FLAG_VSYNC_HINT);
				params.window.SetTargetFPS(refreshRate);
				params.config.updateUseVSYNC(1);
				SetupVideoMenu(params);
			}
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));
	params.menu.selectedIndex = 0;
}

void SetupVisualsMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"Background Color",
		[&params]() {
			params.menu.active = Menu::BGColor;
			SetupBGColorMenu(params);
		}
		});
	params.menu.items.push_back({
		"Image Tint",
		[&params]() {
			params.menu.active = Menu::Tint;
			SetupTintMenu(params);
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));
	params.menu.selectedIndex = 0;
}

void SetupControllerMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"Select Gamepad",
		[&params]() {
			params.menu.active = Menu::Gamepad;
			SetupGamepadMenu(params);
		}
		});
	params.menu.items.push_back({
		"Remap Buttons",
		[&params]() {
			params.menu.active = Menu::RemapButtons;
			SetupRemapMenu(params);
		}
		});
	params.menu.items.push_back({
		"Layout",
		[&params]() {
			params.menu.active = Menu::Layout;
			SetupLayoutMenu(params);
		}
		});
	// Draw Joystick / Remap Joystick — only for layouts with a joystick
	if (params.padcast.getCurrentLayout() == Config::ControllerLayout::N64)
	{
		params.menu.items.push_back({
			"Draw Joystick",
			[&params]() {
				params.menu.active = Menu::DrawJoystick;
				SetupDrawJoystickMenu(params);
			}
		});
		params.menu.items.push_back({
			"Remap Joystick",
			[&params]() {
				params.menu.active = Menu::RemapStick;
			}
		});
	}
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));
	params.menu.selectedIndex = 0;
}

void SetupDrawJoystickMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();

	bool idleOn = (params.config.getDrawJoystickIdle() == 1);

	std::string idleLabel = "Draw idle";
	if (idleOn) idleLabel += " (current)";
	params.menu.items.push_back({
		idleLabel,
		[&params]() {
			params.config.updateDrawJoystickIdle(1);
			SetupDrawJoystickMenu(params);
		}
	});

	std::string movingLabel = "Draw when moving";
	if (!idleOn) movingLabel += " (current)";
	params.menu.items.push_back({
		movingLabel,
		[&params]() {
			params.config.updateDrawJoystickIdle(0);
			SetupDrawJoystickMenu(params);
		}
	});

	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));
	params.menu.selectedIndex = 0;
}

void SetupResolutionMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"1280x720",
		[&params]() { params.window.SetSize(1280, 720); }
		});
	params.menu.items.push_back({
		"960x540",
		[&params]() { params.window.SetSize(960, 540); }
		});
	params.menu.items.push_back({
		"640x360",
		[&params]() { params.window.SetSize(640, 360); }
		});
	params.menu.items.push_back({
		"480x270",
		[&params]() { params.window.SetSize(480, 270); }
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void SetupFPSMenu(MenuContext::MenuParams& params)
{
	bool vsync = params.config.getVSYNC();
	if (vsync)
	{
		params.menu.items.clear();
		params.menu.items.push_back({
			"Can't change FPS\nwhile VSync is on!",
			[&params]() { params.menu.active = Menu::Video; SetupVideoMenu(params); }
			});
		params.menu.items.push_back(createSpacer());
		params.menu.items.push_back(createSpacer());
		params.menu.items.push_back(createBackMenuItem(params));
		params.menu.items.push_back(createCloseMenuItem(params.menu));
		params.menu.selectedIndex = 0;
		return;
	}
	else
	{
	std::string current_fps = std::to_string(params.config.getValue("Window", "TARGET_FPS"));
	std::string current_string = "Current FPS: " + current_fps;
	params.menu.items.clear();
	params.menu.items.push_back({
		current_string,
		[&params]() { params.menu.active = Menu::None; }
		});
	params.menu.items.push_back({
		"30 FPS",
		[&params]() {
			params.window.SetTargetFPS(30);
			params.config.updateTargetFPS(30);
			SetupFPSMenu(params);
		}
		});
	params.menu.items.push_back({
		"60 FPS",
		[&params]() {
			params.window.SetTargetFPS(60);
			params.config.updateTargetFPS(60);
			SetupFPSMenu(params);
		}
		});
	params.menu.items.push_back({
		"90 FPS",
		[&params]() {
			params.window.SetTargetFPS(90);
			params.config.updateTargetFPS(90);
			SetupFPSMenu(params);
		}
		});
	params.menu.items.push_back({
		"120 FPS",
		[&params]() {
			params.window.SetTargetFPS(120);
			params.config.updateTargetFPS(120);
			SetupFPSMenu(params);
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));
	}

	params.menu.selectedIndex = 0;
}

void SetupBGColorMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"Black",
		[&params]() {
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Black));
			params.config.updateUseCustomBG(0);
		}
		});
	params.menu.items.push_back({
		"White",
		[&params]() {
			params.config.updateBGColor(static_cast<int>(BackgroundColor::White));
			params.config.updateUseCustomBG(0);
		}
		});
	params.menu.items.push_back({
		"Red",
		[&params]() {
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Red));
			params.config.updateUseCustomBG(0);
		}
		});
	params.menu.items.push_back({
		"Green",
		[&params]() {
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Green));
			params.config.updateUseCustomBG(0);
		}
		});
	params.menu.items.push_back({
		"Blue",
		[&params]() {
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Blue));
			params.config.updateUseCustomBG(0);
		}
		});
	params.menu.items.push_back({
		"Custom Color",
		[&params]() {
			params.config.updateUseCustomBG(1);
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void SetupTintMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"White",
		[&params]() {
			params.config.updateImageTintPalette(0);
			params.config.updateUseCustomTint(0);
		}
		});
	params.menu.items.push_back({
		"Red",
		[&params]() {
			params.config.updateImageTintPalette(1);
			params.config.updateUseCustomTint(0);
		}
		});
	params.menu.items.push_back({
		"Green",
		[&params]() {
			params.config.updateImageTintPalette(2);
			params.config.updateUseCustomTint(0);
		}
		});
	params.menu.items.push_back({
		"Blue",
		[&params]() {
			params.config.updateImageTintPalette(3);
			params.config.updateUseCustomTint(0);
		}
		});
	params.menu.items.push_back({
		"Custom Tint",
		[&params]() {
			params.config.updateUseCustomTint(1);
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));
	params.menu.selectedIndex = 0;
}

void SetupRemapMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	params.menu.items.push_back({
		"Start Remap",
		[&params]() {
			params.menu.active = Menu::RemapButtons;
		}
		});
	params.menu.items.push_back({
		"Reset to Default",
		[&params]() {
			params.padcast.resetButtonsToDefault();
			params.config.resetButtonMap();
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void SetupGamepadMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();
	for (int i = 0; i < 4; ++i)
	{
		if (raylib::Gamepad::IsAvailable(i))
		{
			std::string menuListing = std::to_string(i) + " > " + params.padcast.getGamepadName(i);
			params.menu.items.push_back({
				menuListing,
				[&params, i]() {
					params.padcast.setGamepadIndex(i);
					params.gamepadIndex = i;
				}
			});
		}
		else
		{
			params.menu.items.push_back({
				"No Gamepads Detected",
				[]() {}
			});
		}
	}

	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void SetupLayoutMenu(MenuContext::MenuParams& params)
{
	params.menu.items.clear();

	auto currentLayout = params.padcast.getCurrentLayout();

	// SNES
	std::string snesLabel = "SNES";
	if (currentLayout == Config::ControllerLayout::SNES)
		snesLabel += " (*)";
	params.menu.items.push_back({
		snesLabel,
		[&params]() {
			params.padcast.setCurrentLayout(Config::ControllerLayout::SNES);
			SetupLayoutMenu(params);
		}
	});

	// N64 (will be added shortly)
	std::string n64Label = "N64";
	if (currentLayout == Config::ControllerLayout::N64)
		n64Label += " (*)";
	params.menu.items.push_back({
		n64Label,
		[&params]() {
			params.padcast.setCurrentLayout(Config::ControllerLayout::N64);
			SetupLayoutMenu(params);
		}
	});

	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void HandleMenuInput(MenuContext::MenuParams& params)
{
	// ----- Menu open/close ----- //
	// a right click or M will open/close the main menu.
	// Space is NOT used here — it's reserved for "skip button" during remap.
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
	    || IsKeyPressed(KEY_M))
	{
		if (params.menu.active == Menu::None)
		{
			params.menu.active = Menu::Main;
			SetupMainMenu(params);
			params.menu.selectedIndex = 0;
		}
		else
		{
			params.menu.active = Menu::None;
		}
		return;
	}

	// ----- Menu navigation ----- //
	if (params.menu.active != Menu::None && !params.menu.items.empty())
	{
		// Keyboard navigation (arrow keys or W/S)
		if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
		{
			params.menu.selectedIndex = (params.menu.selectedIndex + 1) % params.menu.items.size();
		}
		else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
		{
			params.menu.selectedIndex =
				(params.menu.selectedIndex - 1 + params.menu.items.size()) % params.menu.items.size();
		}
		else if (IsKeyPressed(KEY_ENTER))
		{
			params.menu.items[params.menu.selectedIndex].action();
		}

		// Mouse navigation
		Vector2 mousePos = GetMousePosition();
		// Scaling setup for collision box
		MenuScaling ms(params.scaling, 0.7f, 50, 50, 340, 30);
		for (size_t i = 0; i < params.menu.items.size(); ++i)
		{
			Rectangle itemRect = {
				static_cast<float>(ms.x),
				static_cast<float>(ms.y + static_cast<int>(i) * ms.lineHeight),
				static_cast<float>(ms.width),
				static_cast<float>(ms.lineHeight - 5) // Slight padding
			};
			// Detect mouse clicking menu item
			if (CheckCollisionPointRec(mousePos, itemRect))
			{
				params.menu.selectedIndex = static_cast<int>(i);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					params.menu.items[i].action();
				}
			}
		}
	}
}

void DrawMenu(const MenuContext& menu, const ScalingInfo& scaling, const Config& config,
	int baseX, int baseY)
{
	if (menu.active == Menu::None)
	{
		return;
	}

	// Scaling setup
	MenuScaling ms(scaling, 0.7f, baseX, baseY, 340, 30);
	int scaledMenuHeight = static_cast<int>(menu.items.size() * ms.lineHeight + 20 * ms.scale);
	int scaledPadding = static_cast<int>(10 * ms.scale);

	if (menu.active == Menu::Gamepad)
	{
		ms.width = 470;
	}

	// Draw semi-transparent background
	DrawRectangle(ms.x - scaledPadding, ms.y - scaledPadding,
		ms.width, scaledMenuHeight,
		Fade(BLACK, 0.7f)); // %70 opacity

	int defaultFontSize = config.getValue("Font", "DEFAULT_FONT_SIZE");
	int minFontSize = config.getValue("Font", "MIN_FONT_SIZE");
	int fontSize = std::max(static_cast<int>(defaultFontSize * ms.scale), minFontSize);

	// Draw menu items
	for (size_t i = 0; i < menu.items.size(); ++i)
	{
		Color color = (i == menu.selectedIndex ? WHITE : Fade(RAYWHITE, 0.7f));
		DrawText(menu.items[i].label.c_str(),
			ms.x, ms.y + static_cast<int>(i) * ms.lineHeight,
			fontSize, color);
	}
}

// Font cache removed — config reads inlined

void RemapJoystickScreens(MenuContext::MenuParams& params)
{
	// Simplified 4-step stick remap: Up, Right, Down, Left.
	// Each step uses dominant-axis scan across all available axes.
	// Debounce is longer (1 second) and the stick must return to
	// center before the next step can be detected.

	struct StickStep { const char* prompt; int stickDir; };
	static const StickStep kStickSteps[]{
		{ "Move stick UP",    0 },
		{ "Move stick RIGHT", 1 },
		{ "Move stick DOWN",  2 },
		{ "Move stick LEFT",  3 },
	};
	constexpr int kStickStepCount = sizeof(kStickSteps) / sizeof(kStickSteps[0]);

	auto& js = gJoystickRemapState;

	auto doFinish = [&]()
	{
		js.cleanup();
		params.menu.active = Menu::Main;
		params.config.saveConfig();
		SetupMainMenu(params);
	};

	// One-frame init on entry
	if (!js.active)
	{
		js.init();
		for (int a = 0; a < 6; ++a)
		{
			js.axisBaseline[a] = params.padcast.getGamepad().GetAxisMovement(a);
		}
		return;
	}

	if (js.stepIdx >= kStickStepCount)
	{
		doFinish();
		return;
	}

	// Scaling and drawing setup
	float rectScale = params.scaling.effectiveScale(0.8f);
	int rectWidth  = static_cast<int>(420 * rectScale);
	int rectHeight = static_cast<int>(200 * rectScale);
	int rectX = static_cast<int>(
		(params.window.GetWidth() - rectWidth) / 2 + params.scaling.offsetX);
	int rectY = static_cast<int>(
		(params.window.GetHeight() - rectHeight) / 2 + params.scaling.offsetY);

	int defaultFontSize = params.config.getValue("Font", "DEFAULT_FONT_SIZE");
	int minFontSize = params.config.getValue("Font", "MIN_FONT_SIZE");
	int fontSize = std::max(static_cast<int>(defaultFontSize * rectScale), minFontSize);

	DrawRectangle(rectX, rectY, rectWidth, rectHeight, Fade(BLACK, 0.8f));

	const auto& step = kStickSteps[js.stepIdx];
	const char* promptText = step.prompt;

	// Center prompt text
	int textWidth = MeasureText(promptText, fontSize);
	int textY = rectY + (rectHeight - fontSize) / 2;
	int textX = rectX + (rectWidth - textWidth) / 2;
	DrawText(promptText, textX, textY, fontSize, WHITE);

	// Draw "Spacebar to Skip" below prompt
	const char* skipText = "Spacebar to Skip";
	int skipWidth = MeasureText(skipText, static_cast<int>(fontSize * 0.7f));
	DrawText(skipText,
	         rectX + (rectWidth - skipWidth) / 2,
	         textY + fontSize,
	         static_cast<int>(fontSize * 0.7f), Fade(WHITE, 0.5f));

	// Escape to cancel
	if (IsKeyPressed(KEY_ESCAPE))
	{
		doFinish();
		return;
	}

	// Space to skip
	if (IsKeyPressed(KEY_SPACE))
	{
		js.stepIdx++;
		js.waitingCenter = true;
		js.debounce.Reset();
		return;
	}

	// Stick detection
	float axisVals[6]{};
	float bestVal = 0.0f;
	int bestAxis = -1;
	// (allCentered removed — noise on any axis would block detection permanently)

	for (int a = 0; a < 6; ++a)
	{
		axisVals[a] = params.padcast.getGamepad().GetAxisMovement(a);
		float delta = std::abs(axisVals[a] - js.axisBaseline[a]);
		if (delta > bestVal)
		{
			bestVal  = delta;
			bestAxis = a;
		}
	}

	// If stick was off-center and has now returned, reset waitingCenter
	if (js.waitingCenter && bestVal < 0.20f)
		js.waitingCenter = false;



	if (!js.waitingCenter && bestAxis >= 0 && bestVal > 0.4f)
	{
		float val = axisVals[bestAxis] - js.axisBaseline[bestAxis];
		int   dir = step.stickDir;

		// Accept if the direction matches what we expect (sign check)
		bool isPos = (val > 0.0f);
		bool match = false;
		if (dir == 0 && !isPos) match = true;  // Up    (negative Y)
		if (dir == 1 && isPos)  match = true;  // Right (positive X)
		if (dir == 2 && isPos)  match = true;  // Down  (positive Y)
		if (dir == 3 && !isPos) match = true;  // Left  (negative X)

		if (match && js.debounce.CanAcceptInput())
		{
			// Store the axis for this direction
			if (dir == 0 || dir == 2)
				params.config.updateStickYAxis(bestAxis);
			else
				params.config.updateStickXAxis(bestAxis);

			js.stepIdx++;
			js.waitingCenter = true;
			js.debounce.Reset();
		}
	}
}

void RemapButtonScreens(MenuContext::MenuParams& params)
{
	auto& state = gRemapState;

	if (!state.isRemapping)
	{
		state.init();
		state.gamepad = raylib::Gamepad{ params.gamepadIndex };
		return; // let main loop call us again next frame
	}

	// Scaling and drawing setup
	float rectScale = params.scaling.effectiveScale(0.8f);
	int rectWidth = static_cast<int>(420 * rectScale);
	int rectHeight = static_cast<int>(200 * rectScale);
	int rectX = static_cast<int>(
		(params.window.GetWidth() - rectWidth) / 2 + params.scaling.offsetX
		);
	int rectY = static_cast<int>(
		(params.window.GetHeight() - rectHeight) / 2 + params.scaling.offsetY
		);

	int defaultFontSize = params.config.getValue("Font", "DEFAULT_FONT_SIZE");
	int minFontSize = params.config.getValue("Font", "MIN_FONT_SIZE");
	int fontSize = std::max(static_cast<int>(defaultFontSize * rectScale), minFontSize);

	// Draw the background rectangle
	DrawRectangle(rectX, rectY, rectWidth, rectHeight, Fade(BLACK, 0.8f));

	// Select the remap array for the current layout
	auto layout = params.padcast.getCurrentLayout();
	const RemapStep* steps = kSnesRemapSteps;
	int stepCount = kSnesStepCount;
	if (layout == Config::ControllerLayout::N64)
	{
		steps = kN64RemapSteps;
		stepCount = kN64StepCount;
	}

	const char* promptText = "";
	int currentRaylibButton = 0;
	int currentAxisIndex = -1;
	int currentStickDir = -1;
	Config::ButtonConfigKey currentButtonConfig = Config::ButtonConfigKey::DPAD_UP;

	if (state.buttonPromptIndex < stepCount)
	{
		const auto& step = steps[state.buttonPromptIndex];
		promptText = step.prompt;
		currentRaylibButton = step.raylibButton;
		currentAxisIndex = step.axisIndex;
		currentStickDir = step.stickDir;
		currentButtonConfig = step.configKey;
	}
	else
	{
		state.cleanup();
		return;
	}

	// center prompt text
	int textWidth = MeasureText(promptText, fontSize);
	int textHeight = fontSize;
	int textX = rectX + (rectWidth - textWidth) / 2;
	int textY = rectY + (rectHeight - textHeight) / 2;
	DrawText(promptText, textX, textY, fontSize, WHITE);

	if (state.waitingForInput)
	{
		bool inputDetected = false;
		int newButtonPress = -1;

		if (currentStickDir >= 0)
		{
			// Joystick direction scan — find the dominant axis
			int bestAxis = -1;
			float bestVal = 0.5f;  // deadzone threshold
			for (int a = 0; a < 6; ++a)
			{
				float val = state.gamepad.GetAxisMovement(a);
				if (std::abs(val) > bestVal)
				{
					bestAxis = a;
					bestVal = std::abs(val);
				}
			}
			if (bestAxis >= 0)
			{
				float val = state.gamepad.GetAxisMovement(bestAxis);
				// Determine which cardinal direction based on sign and step
				bool match = false;
				bool isPositive = (val > 0.0f);
				if (currentStickDir == 0 && !isPositive) match = true;  // Up    (negative Y)
				if (currentStickDir == 1 && isPositive)  match = true;  // Right (positive X)
				if (currentStickDir == 2 && isPositive)  match = true;  // Down  (positive Y)
				if (currentStickDir == 3 && !isPositive) match = true;  // Left  (negative X)

				if (match)
				{
					// Store which axis drives this direction
					if (currentStickDir == 0 || currentStickDir == 2)
						params.config.updateStickYAxis(bestAxis);
					else
						params.config.updateStickXAxis(bestAxis);

					inputDetected = true;
					newButtonPress = bestAxis; // dummy — not a button
				}
			}
		}
		else if (currentAxisIndex >= 0)
		{
			// Analog input (Z trigger on N64, Axis 5).
			// N64 Z trigger rests at -1.0 and moves to 1.0 when pressed.
			// Detect the press transition: axis crosses above 0.0.
			float axisVal = state.gamepad.GetAxisMovement(currentAxisIndex);
			if (axisVal > 0.0f)
			{
				inputDetected = true;
				newButtonPress = 1; // dummy — axis doesn't produce a button index
			}
		}
		else
		{
			// Check all possible digital button indices (GetButtonPressed is
			// limited to standard indices 0-13; N64 uses indices up to 16).
			for (int btn = 1; btn <= 32; ++btn)
			{
				if (state.gamepad.IsButtonPressed(btn))
				{
					newButtonPress = btn;
					inputDetected = true;
					break;
				}
			}
		}

		if (inputDetected)
		{
			if (state.buttonDebounce.CanAcceptInput())
			{
				// Accept the input
				if (currentAxisIndex < 0)
				{
					params.padcast.setButtonMap(currentRaylibButton, newButtonPress);
					params.config.updateButtonConfig(currentButtonConfig, newButtonPress, layout);
				}
				else
				{
					// For analog triggers, just record the mapping from the defaults
					// (axis has no button index to remap)
					params.config.updateButtonConfig(currentButtonConfig, 1, layout);
				}
				state.buttonPromptIndex++;
				state.waitingForInput = true;
			}
			else
			{
				state.lastAttemptTime = GetTime();
			}
		}

		// Draw "Wait..." or "Spacebar to Skip" below the prompt
		float timeSinceAttempt = GetTime() - state.lastAttemptTime;
		if (timeSinceAttempt < 0.5f && state.lastAttemptTime > 0.0f)
		{
			const char* waitText = "Wait...";
			int waitWidth = MeasureText(waitText, static_cast<int>(fontSize * 0.7f));
			int waitX = rectX + (rectWidth - waitWidth) / 2;
			int waitY = textY + static_cast<int>(fontSize * 1.2f);
			DrawText(waitText, waitX, waitY, static_cast<int>(fontSize * 0.7f), RED);
		}
		else
		{
			const char* skipText = "Spacebar to Skip";
			int skipWidth = MeasureText(skipText, static_cast<int>(fontSize * 0.7f));
			int skipX = rectX + (rectWidth - skipWidth) / 2;
			int skipY = textY + static_cast<int>(fontSize * 1.2f);
			DrawText(skipText, skipX, skipY, static_cast<int>(fontSize * 0.7f), Fade(WHITE, 0.5f));
		}

		// Spacebar to skip the current button
		if (IsKeyPressed(KEY_SPACE))
		{
			state.buttonPromptIndex++;
			state.waitingForInput = true;
			state.buttonDebounce.Reset();
		}

		// Escape keymap if needed
		if (IsKeyPressed(KEY_ESCAPE))
		{
			state.cleanup();
			params.menu.active = Menu::Main;
			params.config.saveConfig();
			params.padcast.loadButtonsFromConfig();  // rebuild cache from saved config
			SetupMainMenu(params);
		}
	}

	// When finished, return to main menu
	if (state.buttonPromptIndex >= stepCount)
	{
		state.cleanup();
		params.menu.active = Menu::Main;
		params.config.saveConfig();
		params.padcast.loadButtonsFromConfig();  // rebuild cache from saved config
		SetupMainMenu(params);
		return;
	}
}
