<!-- priority: low -->
<!-- description: Full project_overview.md — what PadCast is, architecture diagram, directory layout, main loop flow, config system, controller support, build system, roadmap -->

# PadCast Project Overview

A lightweight gamepad visualization tool for streamers. Displays a real-time overlay of controller button presses on screen.

Current version: v0.2.5 | License: BSD 3-Clause

---

## What It Does
PadCast renders a controller image in a resizable window. When the user presses a button on their gamepad, a tinted overlay shape appears on the corresponding button's position on screen. Streamers capture this window via OBS or similar software to show their inputs to viewers.

---

## Architecture

main.cpp → owns PadCast (GamepadTextures, ButtonMap, CachedButtons, drawGamepadButtons, getBGColor, connection stability) → reads Config (INI via mINI, typed getters, validation, saveConfig) → renders Menu system (enum states, SetupXxxMenu, lambda actions)

---

## Directory Layout

```
PadCast/
├── PadCast/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── PadCast.h, config.h, menus.h, debounce.h, pathmanager.h
│   │   └── mini/ini.h (vendored)
│   ├── src/
│   │   ├── main.cpp, PadCast.cpp, config.cpp, menus.cpp, winmain_proxy.cpp
│   ├── resources/images/ (960x540 base resolution PNGs)
│   └── config/config.ini template
├── flatpak/ (manifest + vendored raylib/raylib-cpp tarballs)
├── controllerSVG/ (SNES layout reference SVGs)
└── CMakePresets.json
```

---

## Key Dependencies
- raylib 5.5 (OpenGL 3.3) — window, input, rendering, textures
- raylib-cpp 5.5.0 — C++ wrapper namespace rl
- mINI — single-header INI parser (vendored)

---

## Main Loop Flow
1. HandleMenuInput() — open/close/navigate menus
2. padcast.getTextures() — draw base controller
3. padcast.updateGamepadConnection() — debounced check
4. padcast.drawGamepadButtons() or drawNoGamepadMessage()
5. RemapButtonScreens() — only if in remap mode
6. DrawMenu() — only if menu.active != None

ScalingInfo reconstructed each frame from current window + canvas dims.

---

## Configuration System
- Config reads config.ini on construction via mINI
- validateInt<T>() template validates all values; invalid keys reset to default + trigger save
- getDefault() via static unordered_map
- setValue<T>() template for 7 config keys
- mNeedsSave flag consolidated from old needsSave + mIsDirty
- saveConfig() writes to disk triggered on menu exits and setting changes

---

## Controller Support (Current: SNES)
- 12 buttons: D-pad (4), face (A/B/X/Y/L/R), Select, Start
- ButtonMap: unordered_map raylib constant → display index
- CachedButtons: flat int members for draw-loop speed
- resetButtonMap() hardcodes SNES defaults
- loadButtonMapping() exists but commented-out (future layout-aware loader)
- GamepadTextures: 13 PNGs (1 base + 12 pressed overlays), all SNES-specific

---

## Image System
- Source SVGs in controllerSVG/ (affinity designer)
- Exported to 960x540 PNGs in resources/images/
- raylocator tool used to define button region coordinates
- Base image rendered first, then pressed overlays with tint
- Uniform scale maintains aspect ratio

---

## Menu System
- Right-click / Space / M to open
- Enum stack: None, Main, File, EditOptions, VideoOptions, AudioOptions, RemapOptions, Credits, ConfirmQuit
- Each submenu has SetupXxxMenu() that rebuilds MenuItem vector (label + lambda)
- Arrow/WASD navigation, Enter to select, Esc to close
- Lambda actions mutate Config (via getter) and PadCast (via pointer)
- Font sizes cached with validity flag; InvalidateFontCache() wired to Reload Config File

---

## Cross-Platform

**Windows:** WinMain → main proxy, MSVC-specific flags, AV-friendly build flags
**Linux:** Flatpak (org.padcast.PadCast), RPM, TGZ packaging
**Both:** raylib OpenGL 3.3, portable vs system-installed config path detection

---

## Build System
- CMake presets: linux-release, linux-debug, windows-release, windows-debug
- Ninja generator recommended
- Dependencies fetched via FetchContent (except Flatpak: vendored tarballs)
- Build: `cmake --preset=linux-release && cmake --build --preset=linux-release`

---

## Current State & Roadmap
- v0.2.5 current release ("Under the Hood" update)
- Next: v0.3.0 N64 Controller Support
- Remote branch feature/refactor-gamepad with unmerged work
- Currently on develop branch
- ButtonMap/CachedButtons have commented-out future trigger/joystick support

---

## Known Issues
- Some controllers mislabeled by OS/GLFW (e.g., USB GameCube shown as "XBox Controller")
- Wireless gamepads untested on Linux
- SNES-hardcoded: GamepadTextures, CachedButtons, drawGamepadButtons
- Debug mode logs button/axis data to raylib trace log and stdout
