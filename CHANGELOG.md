# Changelog

All notable changes to PadCast will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## v0.3.0 - 2026-05-20
### Added
- N64 controller support: C-buttons, Z-trigger, joystick rendering.
- Controller layout system (SNES/N64) with menu switching and persistence.
- Per-layout button maps (`[SNES_ButtonMap]`/`[N64_ButtonMap]`) with layout-aware remapping.
- Configurable joystick deadzone (`STICK_DEADZONE` under `[Gamepad]`).
- "Draw Joystick" menu option: idle or only-when-moving modes.
- Spacebar to skip button during remap.
- `FrameTimer` utility (`benchmark.h`) for ad-hoc benchmarking.

### Changed
- `App` class extracted from `main.cpp` (now 8 lines).
- `RemapState` struct replaces static locals in `RemapButtonScreens()`.
- `HandleMenuInput`: Space no longer toggles menu (reserved for remap skip).
- `mScaling` only recalculated on window resize (not every frame).
- Menu navigation guarded against empty item vectors.

### Removed
- Hand-rolled caches for BG color, tint, and font size (benchmarked at ~3μs noise).
- `ResetRemapState()` — was dead code.
- 14 mutable cache member variables from `PadCast.h`.

### Fixed
- `setButtonMap()` now passes layout to `refreshCache()`, preventing C-button/Start overlap after remap.
- Gamepad index respected in remap axis detection.
- Font static cache leak.

---

## v0.2.6 - 2026-05-17
### Added
- App class (`App.h`/`App.cpp`) — encapsulates setup, main loop, and teardown; main.cpp reduced to 8 lines.
- `RemapState` struct — replaces 7 static locals in `RemapButtonScreens()` with `init()`/`cleanup()` methods.
- `FrameTimer` utility (`benchmark.h`) — reusable ad-hoc benchmarking for the draw loop.
- Explicit includes policy — every file now directly includes headers for symbols it uses.

### Changed
- `validateConfig()` — rewritten from ~400 lines of repetitive validation to ~38 lines via `validateInt<T>()` template.
- `getDefault()` — replaced 82-line if/else ladder with `static unordered_map` lookup.
- `RemapButtonScreens()` — state machine cleaned up; `ResetRemapState()` deleted (was dead code — set a different static than the one read).
- `ButtonConfigKey` enum — `updateButtonConfig()` now takes the enum instead of raw string keys.

### Removed
- Hand-rolled cache invalidation for BG color, tint, and font size — benchmarked at ~3μs/frame difference (below measurement noise at 60 FPS). Config values are read directly each frame (in-memory mINI hash lookup).
- `invalidateBGCache()`, `invalidateTintCache()`, `InvalidateFontCache()` and all their call sites.
- 14 mutable cache member variables from `PadCast.h`.
- `ResetRemapState()` function — was dead code.

### Fixed
- Font static cache leak — `DrawMenu()` static locals replaced with file-scope cache + validity flag.
- Duplicate save flags — `needsSave` (local) + `mIsDirty` (member) consolidated into single `mNeedsSave`.
- Boilerplate setters — 9 individual blocks replaced by `setValue()` template.

## v0.2.5 - 2025-10-26
### Added
- Commented out section in CMakeLists.txt for future floating-point calculation optimizations.
- Added VSync option to menu (disabled by default).

### Changed
- Restructured menu system to Video/Visuals/Controller. This is to make space for
future features to be added to the menu.
- Target FPS will be ignored if VSync is enabled.

### Fixed


## [0.2.0] - 2025-09-01
> I didn't keep a changelog for the <v0.2.0 releases, so changelog documentation will start here.
### Features
- **Core Functionality**
  - Lightweight gamepad visualization for streamers
  - Real-time button press visualization with customizable colors
  - Support for SNES-style gamepads (D-pad and face buttons)
  - Automatic USB gamepad detection

- **Customization Features**
  - Variable FPS settings (30, 60, 90, 120 FPS) with custom FPS support
  - Customizable background colors with RGB color picker support
  - Button press tint colors with custom RGB values
  - Button remapping functionality with guided setup process
  - Custom controller image support (.png with transparent backgrounds)
  - Persistent settings storage via config.ini

- **User Interface**
  - In-program menu system (right-click, spacebar, or 'M' to access)
  - Keyboard navigation (W/S, arrow keys, Enter, Escape)
  - Mouse navigation support
  - Live config file reloading
  - Manual controller selection for multiple gamepad support

- **Cross-Platform Support**
  - Windows 10+ support with simple .zip installation
  - Linux support via Flatpak packages
  - Linux support via RPM packages (Fedora/RPM-based distributions)
  - OpenGL 3.3 compatibility

- **Build System**
  - CMake build system with preset configurations
  - Clang/Ninja optimized builds
  - Automatic dependency management via FetchContent
  - Support for both Debug and Release builds
  - C++23 compatibility (GCC 11+, Clang 14+, MSVC 2022+)

- **Advanced Features**
  - Multiple resolution presets with custom resolution support
  - High-resolution image assets (960x540 and 1280x720)
  - Debug mode for controller button mapping
  - SVG source files for custom image creation
  - Custom font size configuration

### Technical Details
- Built with raylib, raylib-cpp, and mINI libraries
- Uses GLFW backend with SDL_GameControllerDB for controller detection
- BSD 3-Clause License

### Known Issues
- Some controllers may be mislabeled by OS/gamepad library (e.g., USB GameCube controllers shown as "XBox Controller")
- Wireless gamepad support untested on Linux builds
