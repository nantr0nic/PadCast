# REASONIX.md — PadCast

## Stack
- C++23, compiled with Clang (Linux) / MSVC (Windows)
- raylib 5.5 (OpenGL 3.3) — FetchContent from GitHub at configure time
- raylib-cpp v5.5.0 — C++ wrapper, also FetchContent
- mINI — INI file parser, vendored at `PadCast/include/mini/ini.h`
- CMake 3.15+ with Ninja generator

## Layout
- `PadCast/src/` — application source (.cpp): main, PadCast, config, menus, winmain_proxy
- `PadCast/include/` — headers (.h): PadCast, config, menus, pathmanager, debounce, scaling, mini/
- `PadCast/resources/` — PNG images for controller and button overlays
- `PadCast/config/` — default config.ini template
- `flatpak/` — Flatpak manifest and vendored raylib/raylib-cpp tarballs
- `controllerSVG/` — SVG reference for SNES controller layout
- `CMakePresets.json` — presets for linux-debug, linux-release, x64-debug, x64-release, etc.
- `build_packs.fish` — one-shot: RPM + TGZ + Flatpak

## Commands
- Configure + build (Linux release):
  `rm -rf out/build/linux-release && cmake --preset=linux-release && cmake --build --preset=linux-release`
  CMake's cache can get stale; always wipe the preset directory first.
- Build Flatpak only: `fish build_flatpak.fish`
- Build all packages: `fish build_packs.fish`
- No test suite, linter, or formatter configured in the repo

## Conventions
- Allman brace style (opening brace on its own line)
- Member variables prefixed with `m` (mConfigPath, mNeedsSave)
- `#include` order: project headers, then standard library
- Header guards: `#ifndef PADCAST_FILENAME_H` / `#define` / `#endif`
- Config validates on load; missing/invalid keys reset to default and trigger save
- Button remapping: `unordered_map` backed by cached flat int lookups for draw-loop speed

## Watch out for
- `out/` is the build output directory — never edit files there
- raylib is fetched at CMake configure time; first build needs network
- Flatpak builds use vendored tarballs in `flatpak/` instead of FetchContent
- Windows build sets `/Zi` (not `/ZI` EditAndContinue) to reduce false-positive AV detections
- Controller type is SNES-hardcoded; non-SNES gamepads only get D-pad + face buttons
- VSCode has format-on-save OFF — no auto-formatting in the repo
