#ifndef PADCAST_MENUS_H
#define PADCAST_MENUS_H

#include <functional>
#include <vector>
#include <PadCast.h>

enum class Menu
{
	None,
	Main,
	Resolution,
	FPS,
	BGColor
	// RemapButtons
};

struct MenuItem
{
	std::string label{};
	std::function<void()> action;
};

struct MenuContext
{
	Menu active{ Menu::None };
	// items will hold menu options for whatever the active menu is
	std::vector<MenuItem> items;
	int selectedIndex{ 0 };

	struct MenuParams
	{
		MenuContext& menu;
		raylib::Window& window;
		Config& config;
		GamepadDisplay& display;

		MenuParams(MenuContext& m, raylib::Window& w, Config& c, GamepadDisplay& d)
			: menu(m), window(w), config(c), display(d)
		{
		}
	};
};

MenuItem createMenuItem(const std::string& label, std::function<void()> action);

MenuItem createBackMenuItem(MenuContext::MenuParams& params);

MenuItem createCloseMenuItem(MenuContext& menu);

MenuItem createSpacer();

void SetupMainMenu(MenuContext::MenuParams& params);

void SetupResolutionMenu(MenuContext::MenuParams& params);

void SetupFPSMenu(MenuContext::MenuParams& params);

void SetupBGColorMenu(MenuContext::MenuParams& params);

void SetupRemapMenu(MenuContext::MenuParams& params);

void HandleMenuInput(MenuContext::MenuParams& params, ScalingInfo& scaling);

void DrawMenu(const MenuContext& menu, const ScalingInfo& scaling, const Config& config,
	int baseX, int baseY);

#endif