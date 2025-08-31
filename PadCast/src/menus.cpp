#include "menus.h"
#include "debounce.h"

#include <string>

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
	// push_back all menu items for active menu
	params.menu.items.push_back({
		"Resolution Menu",
		[&params]() { 
			params.menu.active = Menu::Resolution; 
			SetupResolutionMenu(params); 
		}
		});
	params.menu.items.push_back({
		"FPS",
		[&params]() { 
			params.menu.active = Menu::FPS; 					
			SetupFPSMenu(params); 
		}
		});
	params.menu.items.push_back({
		"Background Color",
		[&params]() { 
			params.menu.active = Menu::BGColor;
			SetupBGColorMenu(params); 
		}
		});
	params.menu.items.push_back({
		"Button Tint Color",
		[&params]() {
			params.menu.active = Menu::Tint;
			SetupTintMenu(params);
		}
		});
	params.menu.items.push_back({
		"Remap Buttons",
		[&params]() { SetupRemapMenu(params); }
		});
	params.menu.items.push_back({
		"Load Controller",
		[&params]() {
			params.menu.active = Menu::Gamepad;
			SetupGamepadMenu(params);
		}
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back({
		"Reload Config File",
		[&params]() {
			params.config.reloadConfig();
			params.padcast.invalidateBGCache();
			params.padcast.invalidateTintCache();
		}
		});
	params.menu.items.push_back(createSpacer());
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
			params.padcast.invalidateBGCache();
		}
		});
	params.menu.items.push_back({
		"White",
		[&params]() { 
			params.config.updateBGColor(static_cast<int>(BackgroundColor::White)); 
			params.config.updateUseCustomBG(0);
			params.padcast.invalidateBGCache();
		}
		});
	params.menu.items.push_back({
		"Red",
		[&params]() { 
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Red));
			params.config.updateUseCustomBG(0);
			params.padcast.invalidateBGCache();
		}
		});
	params.menu.items.push_back({
		"Green",
		[&params]() { 
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Green));
			params.config.updateUseCustomBG(0); 
			params.padcast.invalidateBGCache(); 
		}
		});
	params.menu.items.push_back({
		"Blue",
		[&params]() { 
			params.config.updateBGColor(static_cast<int>(BackgroundColor::Blue));
			params.config.updateUseCustomBG(0); 
			params.padcast.invalidateBGCache(); 
		}
		});
	params.menu.items.push_back({
		"Custom Color",
		[&params]() { 
			params.config.updateUseCustomBG(1);
			params.padcast.invalidateBGCache();
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
			params.padcast.invalidateTintCache();
		}
		});
	params.menu.items.push_back({ 
		"Red",
		[&params]() {
			params.config.updateImageTintPalette(1);
			params.config.updateUseCustomTint(0);
			params.padcast.invalidateTintCache();
		}
		});
	params.menu.items.push_back({ 
		"Green",
		[&params]() {
			params.config.updateImageTintPalette(2);
			params.config.updateUseCustomTint(0);
			params.padcast.invalidateTintCache();
		}
		});
	params.menu.items.push_back({
		"Blue",
		[&params]() {
			params.config.updateImageTintPalette(3);
			params.config.updateUseCustomTint(0);
			params.padcast.invalidateTintCache();
		}
		});
	params.menu.items.push_back({ 
		"Custom Tint",
		[&params]() {
			params.config.updateUseCustomTint(1);
			params.padcast.invalidateTintCache();
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
			ResetRemapState(); 
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

void HandleMenuInput(MenuContext::MenuParams& params)
{
	// ----- Menu open/close ----- //
	// a right click, spacebar, or M will open/close the main menu
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
		|| IsKeyPressed(KEY_SPACE)
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
	if (params.menu.active != Menu::None)
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
		float menuScale = std::max(params.scaling.scale, 0.7f); // don't scale collision box below 70%
		int baseX = 50;
		int baseY = 50;
		int scaledX = static_cast<int>(baseX * menuScale + params.scaling.offsetX);
		int scaledY = static_cast<int>(baseY * menuScale + params.scaling.offsetY);
		int scaledWidth = static_cast<int>(340 * menuScale);
		int scaledLineHeight = static_cast<int>(30 * menuScale);
		for (size_t i = 0; i < params.menu.items.size(); ++i)
		{
			Rectangle itemRect = {
				static_cast<float>(scaledX),
				static_cast<float>(scaledY + static_cast<int>(i) * scaledLineHeight),
				static_cast<float>(scaledWidth),
				static_cast<float>(scaledLineHeight - 5) // Slight padding
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
	float menuScale = std::max(scaling.scale, 0.7f); // don't scale menu font/positions below 70%
	int scaledX = static_cast<int>(baseX * menuScale + scaling.offsetX);
	int scaledY = static_cast<int>(baseY * menuScale + scaling.offsetY);
	int scaledWidth = static_cast<int>(340 * menuScale);
	int scaledLineHeight = static_cast<int>(30 * menuScale);
	int scaledMenuHeight = static_cast<int>(menu.items.size() * scaledLineHeight + 20 * menuScale);
	int scaledPadding = static_cast<int>(10 * menuScale);

	if (menu.active == Menu::Gamepad)
	{
		scaledWidth = 470;
	}

	// Draw semi-transparent background
	DrawRectangle(scaledX - scaledPadding, scaledY - scaledPadding,
		scaledWidth, scaledMenuHeight,
		Fade(BLACK, 0.7f)); // %70 opacity

	// Cached font sizes
	static int defaultFontSize = config.getValue("Font", "DEFAULT_FONT_SIZE");
	static int minFontSize = config.getValue("Font", "MIN_FONT_SIZE");

	// Scaled font size
	int fontSize = std::max(static_cast<int>(defaultFontSize * menuScale), minFontSize);

	// Draw menu items
	for (size_t i = 0; i < menu.items.size(); ++i)
	{
		Color color = (i == menu.selectedIndex ? WHITE : Fade(RAYWHITE, 0.7f));
		DrawText(menu.items[i].label.c_str(),
			scaledX, scaledY + static_cast<int>(i) * scaledLineHeight,
			fontSize, color);
	}
}

void ResetRemapState()
{
	// instead of making the static variables global...
	// not a great solution
	static bool resetRequested = false;
	resetRequested = true;
}

void RemapButtonScreens(MenuContext::MenuParams& params)
{
	static bool isRemapping = false;
	static bool waitingForInput = false;
	static int buttonPromptIndex = 0;
	static raylib::Gamepad remapGamepad(0);
	static DebounceTimer buttonDebounce(0.5f);
	static float lastAttemptTime = 0.0f;

	static bool resetRequested = false;
	if (resetRequested || !isRemapping)
	{
		isRemapping = true;
		waitingForInput = true;
		buttonPromptIndex = 0;
		resetRequested = false;
		buttonDebounce.Reset();
		lastAttemptTime = 0.0f;
		return; // let main loop call us again next frame
	}

	// Scaling and drawing setup
	float rectScale = std::max(params.scaling.scale, 0.8f);
	int rectWidth = static_cast<int>(420 * rectScale);
	int rectHeight = static_cast<int>(200 * rectScale);
	int rectX = static_cast<int>(
		(params.window.GetWidth() - rectWidth) / 2 + params.scaling.offsetX
		);
	int rectY = static_cast<int>(
		(params.window.GetHeight() - rectHeight) / 2 + params.scaling.offsetY
		);

	static int defaultFontSize = params.config.getValue("Font", "DEFAULT_FONT_SIZE");
	static int minFontSize = params.config.getValue("Font", "MIN_FONT_SIZE");
	int fontSize = std::max(static_cast<int>(defaultFontSize * rectScale), minFontSize);

	// Draw the background rectangle
	DrawRectangle(rectX, rectY, rectWidth, rectHeight, Fade(BLACK, 0.8f));

	const char* promptText = ""; // cuz raylib's DrawText() argument asks for a const char*
	int currentRaylibButton = 0;
	std::string currentButtonConfig;

	switch (buttonPromptIndex)
	{
		// case #'s match order of buttons in unordered_map buttonIndex
	case 0:
		promptText = "Press D-pad UP";
		currentRaylibButton = GAMEPAD_BUTTON_LEFT_FACE_UP;
		currentButtonConfig = "DPAD_UP";
		break;
	case 1:
		promptText = "Press D-pad RIGHT";
		currentRaylibButton = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
		currentButtonConfig = "DPAD_RIGHT";
		break;
	case 2:
		promptText = "Press D-pad DOWN";
		currentRaylibButton = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
		currentButtonConfig = "DPAD_DOWN";
		break;
	case 3:
		promptText = "Press D-pad LEFT";
		currentRaylibButton = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
		currentButtonConfig = "DPAD_LEFT";
		break;
	case 4:
		promptText = "Press X";
		currentRaylibButton = GAMEPAD_BUTTON_RIGHT_FACE_UP;
		currentButtonConfig = "X_BUTTON";
		break;
	case 5:
		promptText = "Press A";
		currentRaylibButton = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;
		currentButtonConfig = "A_BUTTON";
		break;
	case 6:
		promptText = "Press B";
		currentRaylibButton = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
		currentButtonConfig = "B_BUTTON";
		break;
	case 7:
		promptText = "Press Y";
		currentRaylibButton = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;
		currentButtonConfig = "Y_BUTTON";
		break;
	case 8:
		promptText = "Press LEFT Shoulder";
		currentRaylibButton = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
		currentButtonConfig = "L_BUTTON";
		break;
	case 9:
		promptText = "Press RIGHT Shoulder";
		currentRaylibButton = GAMEPAD_BUTTON_RIGHT_TRIGGER_1;
		currentButtonConfig = "R_BUTTON";
		break;
	case 10:
		promptText = "Press Select";
		currentRaylibButton = GAMEPAD_BUTTON_MIDDLE_LEFT;
		currentButtonConfig = "SELECT";
		break;
	case 11:
		promptText = "Press Start";
		currentRaylibButton = GAMEPAD_BUTTON_MIDDLE_RIGHT;
		currentButtonConfig = "START";
		break;
	default:
		// Finished remapping
		isRemapping = false;
		waitingForInput = false;
		buttonPromptIndex = 0;
		return;
	}

	// center prompt text
	int textWidth = MeasureText(promptText, fontSize);
	int textHeight = fontSize;
	int textX = rectX + (rectWidth - textWidth) / 2;
	int textY = rectY + (rectHeight - textHeight) / 2;
	DrawText(promptText, textX, textY, fontSize, WHITE);

	if (waitingForInput)
	{
		int newButtonPress = remapGamepad.GetButtonPressed();
		if (newButtonPress > 0)
		{
			if (buttonDebounce.CanAcceptInput())
			{
				// Accept the input
				params.padcast.setButtonMap(currentRaylibButton, newButtonPress);
				params.config.updateButtonConfig(currentButtonConfig, newButtonPress);
				buttonPromptIndex++;
				waitingForInput = true;
			}
			else
			{
				lastAttemptTime = GetTime();
			}
		}

		// Draw "Wait..." message if user pressed too quickly
		float timeSinceAttempt = GetTime() - lastAttemptTime;
		if (timeSinceAttempt < 1.0f && lastAttemptTime > 0.0f)
		{
			const char* waitText = "Wait...";
			int waitWidth = MeasureText(waitText, static_cast<int>(fontSize * 0.7f));
			int waitX = rectX + (rectWidth - waitWidth) / 2;
			int waitY = textY + static_cast<int>(fontSize * 1.2f);
			DrawText(waitText, waitX, waitY, static_cast<int>(fontSize * 0.7f), RED);
		}

		// Escape keymap if needed
		if (IsKeyPressed(KEY_ESCAPE))
		{
			isRemapping = false;
			waitingForInput = false;
			buttonPromptIndex = 0;
			params.menu.active = Menu::Main;
			SetupMainMenu(params);
		}
	}

	// When finished, return to main menu
	if (buttonPromptIndex >= 12)
	{
		isRemapping = false;
		waitingForInput = false;
		buttonPromptIndex = 0;
		params.menu.active = Menu::Main;
		params.config.saveConfig();
		SetupMainMenu(params);
		return;
	}
}