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
	// RemapButtons,
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
};

void SetupMainMenu(MenuContext& menu, raylib::Window& window, Config& config);

void SetupResolutionMenu(MenuContext& menu, raylib::Window& window, Config& config);

void SetupFPSMenu(MenuContext& menu, raylib::Window& window, Config& config);

void HandleMenuInput(MenuContext& menu, raylib::Window& window, Config& config, ScalingInfo& scaling);

void DrawMenu(const MenuContext& menu, const ScalingInfo& scaling, const Config& config,
	int baseX, int baseY);

#endif