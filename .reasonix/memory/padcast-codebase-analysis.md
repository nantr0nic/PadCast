<!-- priority: low -->
<!-- description: Full codebase_analysis.md — file-by-file breakdown, optimization patterns, refactoring targets, build notes, key files index -->

# PadCast Codebase Analysis

Technical deep-dive for developers working on this code.
Last analyzed: branch `develop`, SHA HEAD of `develop`.

---

## File-by-File Breakdown

### `/PadCast/src/main.cpp` (Entry Point)

| Aspect | Detail |
|---|---|
| **Role** | Window setup, main loop, frame orchestration |
| **Key variables** | `mainConfig` (Config), `padcast` (PadCast), `menu` (MenuContext), `scaling` (ScalingInfo), `gamepadIndex` (static int) |
| **Frame budget** | ~15 frames between gamepad connection checks (every ~250ms at 60fps) |

**Data flow per frame:**
HandleMenuInput() → padcast.getTextures() → padcast.updateGamepadConnection() → padcast.drawGamepadButtons() or drawNoGamepadMessage() → RemapButtonScreens() (if in remap mode) → DrawMenu() (if menu.active != None)

**Notable:** ScalingInfo is reconstructed every frame from current window dimensions + canvas dimensions.

### PadCast.h / PadCast.cpp

**GamepadTextures struct** — Loads 13 PNGs in constructor, all SNES-specific. Would need parallel sets for other controllers.

**ButtonMap struct** — `unordered_map<int, int>` buttonIndex (raylib constant → display index) + defaultSNESIndex. remapButton() updates map entry. GAMEPAD_BUTTON_LEFT_TRIGGER_2 and RIGHT_TRIGGER_2 have commented-out placeholder code for future N64/GC.

**CachedButtons struct** — Flattens unordered_map into individual int members for fast per-frame lookup. refreshCache() iterates buttonIndex.

**PadCast class** — updateGamepadConnection (debounced), drawGamepadButtons (tinted overlays), drawNoGamepadMessage, getBGColor (lazy cached), loadButtonsFromConfig (constructor), findGamepadIndex. Hand-rolled cache invalidation pattern throughout.

### config.h / config.cpp

**Config class** — lazy-loaded from INI via mINI. validateInt<T>() template (~36 call sites), setValue<T>() template (7 call sites + manual). mNeedsSave flag. getDefault() via static unordered_map. ButtonConfigKey enum. resetButtonMap() hardcodes SNES indices. loadButtonMapping() commented-out for future layout-aware loader.

### menus.h / menus.cpp

**Menu enum** — None, Main, File, EditOptions, VideoOptions, AudioOptions, RemapOptions, Credits, ConfirmQuit.

**MenuContext** — vector of MenuItem (label + lambda). Each submenu has SetupXxxMenu() that rebuilds items. HandleMenuInput() (keyboard + mouse navigation). DrawMenu() with cached fonts. RemapButtonScreens() with static locals.

### pathmanager.h
PathManager class — all static methods. getResourcePath(), getConfigPath(), getPortableConfigPath(). Portable mode: exe-relative. System mode: XDG on Linux, %APPDATA% on Windows. Fallback-by-design.

### debounce.h
DebounceTimer class — simple state machine for connection stability.

### CMakeLists.txt
Sub-project build. raylib 5.5 + raylib-cpp 5.5.0 via FetchContent. mINI vendored. MSVC-specific flags (/guard:cf, /GL, /LTCG). Flatpak: separate deps.

---

## Optimization Patterns

- CachedButtons flattens map → ints for draw loop
- Background color lazily cached with dirty tracking
- Tint color same pattern in drawGamepadButtons()
- Font size cached with validity flag + InvalidateFontCache()
- All caching is hand-rolled (no memoize library)
- static locals in DrawMenu() and RemapButtonScreens() for font sizes

---

## Refactoring Targets

### High Priority
1. Abstract controller types (ControllerProfile) — Not started
2. ButtonMap generalization (variable button count) — Not started
3. Axis/joystick rendering — Not started

### Medium Priority
4. resetButtonMap() layout-awareness — Pending (v0.3.0)
5. Static locals in RemapButtonScreens() — Not started
6. Caching system cleanup — Not started

### Low Priority
7. Debug mode commented code cleanup — Not started

### Completed
- validateConfig() — ~400 lines → template-driven, ~38 lines
- getDefault() — 82-line if/else ladder → static unordered_map lookup
- Duplicate save flags — needsSave + mIsDirty consolidated into mNeedsSave
- Boilerplate setters — 9 blocks replaced by setValue() template
- Raw string button keys — updateButtonConfig() now takes ButtonConfigKey enum
- Font static cache leak — replaced static locals with file-scope cache + InvalidateFontCache()

---

## Build & Test Notes

- Presets: linux-release, linux-debug, windows-release, windows-debug
- Compilers: GCC 11+, Clang 14+, MSVC 2022+ (C++23)
- Ninja recommended
- Debug builds show console; Release don't
- First build slow (FetchContent compiles raylib + raylib-cpp from source)

---

## Key Files Cheat Sheet

| Need To... | Open This File |
|---|---|
| Understand the frame loop | PadCast/src/main.cpp |
| Add a new controller type | PadCast/include/PadCast.h + PadCast/src/PadCast.cpp |
| Add a config setting | PadCast/include/config.h + config.cpp |
| Add a menu option | PadCast/include/menus.h + menus.cpp |
| Change resource paths | PadCast/include/pathmanager.h |
| Add packaging format | Top-level CMakeLists.txt or flatpak/ |
