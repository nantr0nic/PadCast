#include <menus.h>

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
		[&params]() { params.menu.active = Menu::Resolution; 
									SetupResolutionMenu(params); }
		});
	params.menu.items.push_back({
		"FPS",
		[&params]() { params.menu.active = Menu::FPS; 
									SetupFPSMenu(params); }
		});
	params.menu.items.push_back({
		"Background Color",
		[&params]() { params.menu.active = Menu::BGColor;
									SetupBGColorMenu(params); }
		});
	/*
	params.menu.items.push_back({
		"Remap Buttons",
		[&menu]() { params.menu.active = Menu::RemapButtons; SetupRemapMenu(menu); }
	});
	*/
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
		[&params]() { params.config.updateBGColor(static_cast<int>(BackgroundColor::Black)); }
		});
	params.menu.items.push_back({
		"White",
		[&params]() { params.config.updateBGColor(static_cast<int>(BackgroundColor::White)); }
		});
	params.menu.items.push_back({
		"\"Raywhite\"",
		[&params]() { params.config.updateBGColor(static_cast<int>(BackgroundColor::Raywhite)); }
		});
	params.menu.items.push_back({
		"Red",
		[&params]() { params.config.updateBGColor(static_cast<int>(BackgroundColor::Red)); }
		});
	params.menu.items.push_back({
		"Green",
		[&params]() { params.config.updateBGColor(static_cast<int>(BackgroundColor::Green)); }
		});
	params.menu.items.push_back({
		"Blue",
		[&params]() { params.config.updateBGColor(static_cast<int>(BackgroundColor::Blue)); }
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
		[]() {}
		});
	params.menu.items.push_back({
		"Reset to Default",
		[]() { }
		});
	params.menu.items.push_back(createSpacer());
	params.menu.items.push_back(createBackMenuItem(params));
	params.menu.items.push_back(createCloseMenuItem(params.menu));

	params.menu.selectedIndex = 0;
}

void HandleMenuInput(MenuContext::MenuParams& params, ScalingInfo& scaling)
{
	// ----- Menu open/close ----- //
	// a right click, spacebar, or M will open the main menu
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
		// Keyboard navigation
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
			// menu.active = Menu::None;
		}

		// Mouse navigation
		Vector2 mousePos = GetMousePosition();
		// Scaling setup
		float menuScale = std::max(scaling.scale, 0.8f); // don't scale menu font/positions below 80%
		int baseX = 50;
		int baseY = 50;
		int scaledX = static_cast<int>(baseX * menuScale + scaling.offsetX);
		int scaledY = static_cast<int>(baseY * menuScale + scaling.offsetY);
		int scaledWidth = static_cast<int>(200 * menuScale);
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
	float menuScale = std::max(scaling.scale, 0.8f); // don't scale menu font/positions below 80%
	int scaledX = static_cast<int>(baseX * menuScale + scaling.offsetX);
	int scaledY = static_cast<int>(baseY * menuScale + scaling.offsetY);
	int scaledWidth = static_cast<int>(300 * menuScale);
	int scaledLineHeight = static_cast<int>(30 * menuScale);
	int scaledMenuHeight = static_cast<int>(menu.items.size() * scaledLineHeight + 20 * menuScale);
	int scaledPadding = static_cast<int>(10 * menuScale);

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
		Color color = (i == menu.selectedIndex ? RAYWHITE : Fade(WHITE, 0.8f));
		DrawText(menu.items[i].label.c_str(),
			scaledX, scaledY + static_cast<int>(i) * scaledLineHeight,
			fontSize, color);
	}
}