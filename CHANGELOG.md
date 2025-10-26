# Changelog

All notable changes to PadCast will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
