#include <menus.h>

void SetupMainMenu(MenuContext& menu, raylib::Window& window, Config& config)
{
	menu.items.clear();
	// push_back all menu items for active menu
	menu.items.push_back({
		"Resolution Menu",
		[&menu, &window, &config]() { menu.active = Menu::Resolution; 
									SetupResolutionMenu(menu, window, config); }
		});
	menu.items.push_back({
		"FPS",
		[&menu, &window, &config]() { menu.active = Menu::FPS; SetupFPSMenu(menu, window, config); }
		});
	/*
	menu.items.push_back({
		"Remap Buttons",
		[&menu]() { menu.active = Menu::RemapButtons; SetupRemapMenu(menu); }
	});
	*/
	menu.items.push_back({
		"Close",
		[&menu]() { menu.active = Menu::None; }
		});

	menu.selectedIndex = 0;
}

void SetupResolutionMenu(MenuContext& menu, raylib::Window& window, Config& config)
{
	menu.items.clear();
	menu.items.push_back({
		"1280x720",
		[&window]() { window.SetSize(1280, 720); }
		});
	menu.items.push_back({
		"960x540",
		[&window]() { window.SetSize(960, 540); }
		});
	menu.items.push_back({
		"640x360",
		[&window]() { window.SetSize(640, 360); }
		});
	menu.items.push_back({
		"480x270",
		[&window]() { window.SetSize(480, 270); }
		});
	menu.items.push_back({
		"Back",
		[&menu, &window, &config]() { menu.active = Menu::Main; SetupMainMenu(menu, window, config); }
		});
	menu.items.push_back({
		"Close",
		[&menu]() { menu.active = Menu::None; }
		});

	menu.selectedIndex = 0;
}

void SetupFPSMenu(MenuContext& menu, raylib::Window& window, Config& config)
{
	std::string current_fps = std::to_string(config.getValue("Window", "TARGET_FPS"));
	std::string current_string = "Current FPS: " + current_fps;
	menu.items.clear();
	menu.items.push_back({
		current_string,
		[&menu]() { menu.active = Menu::None; }
		});
	menu.items.push_back({
		"30 FPS",
		[&menu, &window, &config]() {
			window.SetTargetFPS(30);
			config.updateTargetFPS(30);
			SetupFPSMenu(menu, window, config);
		}
		});
	menu.items.push_back({
		"60 FPS",
		[&menu, &window, &config]() {
			window.SetTargetFPS(60);
			config.updateTargetFPS(60);
			SetupFPSMenu(menu, window, config);
		}
		});
	menu.items.push_back({
		"90 FPS",
		[&menu, &window, &config]() {
			window.SetTargetFPS(90);
			config.updateTargetFPS(90);
			SetupFPSMenu(menu, window, config);
		}
		});
	menu.items.push_back({
		"120 FPS",
		[&menu, &window, &config]() {
			window.SetTargetFPS(120);
			config.updateTargetFPS(120);
			SetupFPSMenu(menu, window, config);
		}
		});
	menu.items.push_back({
		"Back",
		[&menu, &window, &config]() { menu.active = Menu::Main; 
									SetupMainMenu(menu, window, config); }
		});
	menu.items.push_back({
		"Close",
		[&menu]() { menu.active = Menu::None; }
		});
}

void HandleMenuInput(MenuContext& menu, raylib::Window& window, 
					Config& config, ScalingInfo& scaling)
{
	// Menu open/close
	// a right click, spacebar, or M will open the main menu
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
		|| IsKeyPressed(KEY_SPACE)
		|| IsKeyPressed(KEY_M))
	{
		if (menu.active == Menu::None)
		{
			menu.active = Menu::Main;
			SetupMainMenu(menu, window, config);
			menu.selectedIndex = 0;
		}
		else
		{
			menu.active = Menu::None;
		}
		return;
	}

	// Menu navigation
	if (menu.active != Menu::None)
	{
		// Keyboard navigation
		if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
		{
			menu.selectedIndex = (menu.selectedIndex + 1) % menu.items.size();
		}
		else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
		{
			menu.selectedIndex =
				(menu.selectedIndex - 1 + menu.items.size()) % menu.items.size();
		}
		else if (IsKeyPressed(KEY_ENTER))
		{
			menu.items[menu.selectedIndex].action();
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
		for (size_t i = 0; i < menu.items.size(); ++i)
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
				menu.selectedIndex = static_cast<int>(i);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					menu.items[i].action();
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
	int scaledWidth = static_cast<int>(200 * menuScale);
	int scaledLineHeight = static_cast<int>(30 * menuScale);
	int scaledMenuHeight = static_cast<int>(menu.items.size() * scaledLineHeight + 20 * menuScale);
	int scaledPadding = static_cast<int>(10 * menuScale);

	// Draw semi-transparent background
	DrawRectangle(scaledX - scaledPadding, scaledY - scaledPadding,
		scaledWidth, scaledMenuHeight,
		Fade(BLACK, 0.7f)); // %70 opacity

	// Scaled font size
	int fontSize = std::max(
		static_cast<int>(config.getValue("Font", "DEFAULT_FONT_SIZE") * menuScale),
		config.getValue("Font", "MIN_FONT_SIZE")
	);

	// Draw menu items
	for (size_t i = 0; i < menu.items.size(); ++i)
	{
		Color color = (i == menu.selectedIndex ? RAYWHITE : Fade(WHITE, 0.8f));
		DrawText(menu.items[i].label.c_str(),
			scaledX, scaledY + static_cast<int>(i) * scaledLineHeight,
			fontSize, color);
	}
}