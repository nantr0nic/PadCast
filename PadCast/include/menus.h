#ifndef PADCAST_MENUS_H
#define PADCAST_MENUS_H

#ifdef _WIN32
	#define NOGDI
	#define NOUSER
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#endif

#include <PadCast.h>

#ifdef _WIN32
	#undef NOGDI
	#undef NOUSER
#endif

#include <functional>
#include <vector>

enum class Menu
{
	None,
	Main,
	Video,
	Visuals,
	Controller,
	Resolution,
	FPS,
	BGColor,
	Tint,
	RemapButtons,
	Gamepad
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
		PadCast& padcast;
		ScalingInfo& scaling;
		int& gamepadIndex;

		MenuParams(MenuContext& men, raylib::Window& win, Config& con, 
				   PadCast& pad, ScalingInfo& sca, int& gpIndex)
			: menu(men), window(win), config(con), padcast(pad), scaling(sca), gamepadIndex(gpIndex)
		{
		}
	};
};

MenuItem createMenuItem(const std::string& label, std::function<void()> action);

MenuItem createBackMenuItem(MenuContext::MenuParams& params);

MenuItem createCloseMenuItem(MenuContext& menu);

MenuItem createSpacer();

void SetupMainMenu(MenuContext::MenuParams& params);

void SetupVideoMenu(MenuContext::MenuParams& params);

void SetupVisualsMenu(MenuContext::MenuParams& params);

void SetupControllerMenu(MenuContext::MenuParams& params);

void SetupResolutionMenu(MenuContext::MenuParams& params);

void SetupFPSMenu(MenuContext::MenuParams& params);

void SetupBGColorMenu(MenuContext::MenuParams& params);

void SetupTintMenu(MenuContext::MenuParams& params);

void SetupRemapMenu(MenuContext::MenuParams& params);

void SetupGamepadMenu(MenuContext::MenuParams& params);

void HandleMenuInput(MenuContext::MenuParams& params);

void DrawMenu(const MenuContext& menu, const ScalingInfo& scaling, const Config& config,
	int baseX, int baseY);

void ResetRemapState();

void RemapButtonScreens(MenuContext::MenuParams& params);

#endif