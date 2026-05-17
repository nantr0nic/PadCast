# PadCast Codebase Analysis

> Technical deep-dive for developers working on this code.
> Last analyzed: branch `develop`, SHA HEAD of `develop`.

---

## File-by-File Breakdown

### `/PadCast/src/main.cpp` (Entry Point)

| Aspect | Detail |
|---|---|
| **Role** | Window setup, main loop, frame orchestration |
| **Key variables** | `mainConfig` (Config), `padcast` (PadCast), `menu` (MenuContext), `scaling` (ScalingInfo), `gamepadIndex` (static int) |
| **Frame budget** | ~15 frames between gamepad connection checks (every ~250ms at 60fps) |

**Data flow per frame:**
```
HandleMenuInput()          ← menu open/close, navigation
→ padcast.getTextures()    ← draws base controller
→ padcast.updateGamepadConnection()
→ padcast.drawGamepadButtons() or drawNoGamepadMessage()
→ RemapButtonScreens()     ← only if in remap mode
→ DrawMenu()               ← only if menu.active != None
```

**Notable:** ScalingInfo is **reconstructed every frame** from current window
dimensions + canvas dimensions. The `menuParams` struct bundles all mutable
references passed through the menu system.

---

### `/PadCast/include/PadCast.h` + `/PadCast/src/PadCast.cpp`

#### GamepadTextures struct
- Loads all 13 PNGs in constructor via `PathManager::getResourcePath()`
- 1 base + 12 pressed overlays
- **All SNES-specific** — would need parallel sets for other controllers

#### ButtonMap struct
```cpp
std::unordered_map<int, int> buttonIndex;  // raylib constant → display index
std::unordered_map<int, int> defaultSNESIndex;  // factory defaults
```
- `remapButton(raylibButton, newIndex)` — updates map entry
- Default SNES mapping hardcoded in initializer list
- `GAMEPAD_BUTTON_LEFT_TRIGGER_2` (10) and `GAMEPAD_BUTTON_RIGHT_TRIGGER_2` (12)
  have **commented-out placeholder code** for future use (N64 Z button / GC triggers)

#### CachedButtons struct
- Flattens the unordered_map into individual int members for fast per-frame lookup
- `refreshCache(const ButtonMap&)` — iterates buttonIndex and populates fields
- Avoids map lookup overhead in the hot draw loop

#### PadCast class

**Key methods:**

| Method | Called | Purpose |
|---|---|---|
| `updateGamepadConnection()` | Every ~15 frames | Debounces connection state changes via STABILITY_THRESHOLD |
| `drawGamepadButtons()` | Every frame (when connected) | Renders all pressed button overlays with tint |
| `drawNoGamepadMessage()` | Every frame (when disconnected) | Shows "No Gamepad Connected" text |
| `getBGColor()` | Every frame (lazy) | Returns cached Color, invalidated by menu actions |
| `loadButtonsFromConfig()` | Constructor | Reads ButtonMap section from config.ini |
| `findGamepadIndex()` | Manual trigger | Scans gamepads 0-3, logs names |

**Caching pattern used throughout:**
```
mutable member + validity flag + "last" values for comparison
→ getBGColor() checks if config values changed
→ Only reconstructs Color object when needed
→ Same pattern for tint color
```

This is a hand-rolled cache-invalidation approach. It works but adds complexity.

---

### `/PadCast/include/config.h` + `/PadCast/src/config.cpp`

#### Config class

- Wraps `mINI::INIFile` + `mINI::INIStructure`
- **16 config keys** across 6 sections
- `DefaultValues` struct with constexpr defaults
- `SNESMapDefaults` struct for button map defaults
- `getValue(section, key)` — reads with fallback to `getDefault()`
- `getDefault(section, key)` — uses a `static std::unordered_map` lookup
- `mNeedsSave` flag — set by `validateInt()`, setters, and `resetButtonMap()`;
  checked by `saveConfig()` to avoid unnecessary disk writes
- `setValue(section, key, value)` template — all public setters delegate to it
- `ButtonConfigKey` enum (public, 12 values) — type-safe alternative to raw
  string keys in `updateButtonConfig()`. A `switch` maps enum → INI key name.
- `validateInt(section, key, default, validator)` template — compact per-key
  validation; `validateConfig()` is now ~38 lines instead of ~400.

**validateConfig()** — each config key is now one line:
```cpp
validateInt("Window", "INITIAL_WINDOW_WIDTH", DefaultValues::INITIAL_WINDOW_WIDTH,
            [](int val) { return val > 0; });
```

---

### `/PadCast/include/menus.h` + `/PadCast/src/menus.cpp`

#### Menu enum
```cpp
enum class Menu { None, Main, Video, Visuals, Controller, Resolution,
                  FPS, BGColor, Tint, RemapButtons, Gamepad };
```

#### MenuContext
- `active` — current menu enum value
- `items` — vector of MenuItem (label + action lambda)
- `selectedIndex` — keyboard/mouse navigation state
- `MenuParams` — bundle of references passed through the system

#### MenuItem
```cpp
struct MenuItem {
    std::string label;
    std::function<void()> action;  // lambda captures params by reference
};
```

#### Pattern for each submenu:
```
1. Clear items vector
2. Push MenuItem objects with lambdas
3. Lambdas capture MenuParams& by reference
4. Action either:
   a. Changes menu.active + calls another Setup function (submenu navigation)
   b. Mutates Config/PadCast (setting change)
   c. Calls invalidateBGCache/invalidateTintCache (visual update)
```

#### HandleMenuInput()
- Opens/closes menu on right-click, Space, M
- Keyboard: Up/Down or W/S → navigation, Enter → action
- Mouse: collision detection on scaled item rectangles
- Returns early if menu just opened/closed

#### DrawMenu()
- Semi-transparent black background (`Fade(BLACK, 0.7f)`)
- Scaled font size (clamped at MIN_FONT_SIZE)
- Menu scale clamped at 0.7 minimum
- Highlighted selected item in WHITE, others in faded RAYWHITE

#### RemapButtonScreens()
- Uses **static local variables** for state (isRemapping, buttonPromptIndex, etc.)
- 12-step guided walkthrough, 500ms debounce between presses
- Draws centered prompt text + "Wait..." warning on rapid input
- Escape cancels back to main menu
- On completion, saves config and returns to main menu

---

### `/PadCast/include/pathmanager.h`

#### PathManager class (all static methods)

**Two modes:**

1. **Portable mode** — if `resources/` or `config/` exists next to exe
   - Config: `{exe_dir}/config/config.ini`
   - Resources: `{exe_dir}/resources/{path}`

2. **System install mode** (Linux)
   - Config: `$XDG_CONFIG_HOME/padcast/config.ini` or `~/.config/padcast/config.ini`
   - Copies default from `/usr/share/padcast/config/config.ini` if user config missing
   - Resources: `PADCAST_DATA_DIR/resources/{path}` (default `/usr/share/padcast`)

**Executable path detection:**
- Windows: `GetModuleFileNameW(NULL, ...)` (wide char, 32767 byte buffer)
- Linux: `read_symlink("/proc/self/exe")`
- Fallback: `current_path()`

---

### `/PadCast/include/debounce.h`

#### DebounceTimer class
- Single-purpose: prevents rapid re-triggering of button presses
- `CanAcceptInput()` — returns true if `debounceTime` (default 500ms) has elapsed
- Used exclusively in button remapping

---

### `/PadCast/src/winmain_proxy.cpp`

- Trivial: wraps WinMain → main() for Windows GUI apps
- Only compiled on `_WIN32`

---

### `/PadCast/CMakeLists.txt`

Key details:
- C++23 standard
- Links raylib + raylib-cpp
- MSVC-specific anti-virus flags (`/guard:cf`, `/GL`, `/LTCG`)
- Floting-point optimization section **currently commented out** for future use
- Copies resources + config to binary directory during build
- Linux install targets: binary, resources, desktop file, icon, metainfo
- CPack for RPM packaging
- WIN32_EXECUTABLE property suppresses console on Release builds

---

## Optimization Patterns

The codebase uses several hand-rolled optimization techniques:

### 1. Lazy Caching with Invalidation
```
mCachedValue / mCacheValid flag
  ↓ Menu action calls invalidate()
  ↓ Next getter call: cache miss → rebuild
  ↓ Subsequent calls: cache hit → return cached
```

Used for: `getBGColor()`, tint color in `drawGamepadButtons()`

### 2. Flattened Button Cache
```
ButtonMap (unordered_map) → CachedButtons (flat ints)
  ↓ refreshCache() called on config load or remap
  ↓ Draw loop reads direct int members (no map lookup)
```

### 3. Connection Stability Filter
```
updateGamepadConnection():
  If state unchanged → reset counter
  If state changed → increment counter
  Only accept change after STABILITY_THRESHOLD frames
Prevents flickering when controllers are reconnecting
```

### 4. Frame-Skipped Connection Checks
```
gamepadCheckCounter increments each frame
Only actually checks IsAvailable() every 15 frames
Reduces overhead of the availability check
```

---

## Constraints & Limitations for Adding New Controllers

### Hard-coded SNES assumptions:

1. **GamepadTextures** — loads exactly 13 SNES-specific PNGs by name
   - No mechanism for controller-type switching
   - No texture atlas or generic button slot system

2. **CachedButtons** — has exactly 12 fields for SNES buttons
   - No joystick/analog support
   - No analog trigger support (triggers are binary shoulder buttons only)
   - Commented-out placeholders for trigger_index_2 but unused

3. **drawGamepadButtons()** — hardcoded if/else chain for each SNES button
   - No joystick rendering (would need rotation/position animation)
   - No analog axis reading (only `IsButtonDown()`)

4. **Config INI keys** — `config.h` has `SNESMapDefaults`
   - New controllers need new default maps

5. **Remap menu** — 12-step sequence hardcoded for SNES buttons
   - N64 has C-buttons, Z button, analog stick
   - GameCube has analog triggers, joystick, D-pad
   - Would need configurable remap sequence per controller type

## Controller Layout Awareness (Design Discussion)

This section captures the planned approach for multi-controller support, which
will be needed for N64 (v0.3.0), GameCube (v0.4.0), and beyond.

### Strategy: per-layout INI sections

Each controller layout gets its own `[ButtonMap_*]` section in `config.ini`:
```ini
[ButtonMap]        ; SNES (default, backward-compatible)
[ButtonMap_N64]    ; Nintendo 64
[ButtonMap_GC]     ; GameCube
```

A `ControllerLayout` enum drives section selection:
```cpp
enum class ControllerLayout { SNES, N64, GameCube };
```

### What's already in place

- **`ButtonConfigKey` enum** (in `config.h`) — 12 type-safe button names used
  by `updateButtonConfig()`. For N64/GC, this enum can be extended or a
  per-layout enum created.
- **`validateInt()` template** — adding a new INI key for a layout-specific
  button map is one line in `validateConfig()`.
- **`setValue()` template** — layout-aware setters will delegate to it.
- **Commented-out `loadButtonMapping(ControllerLayout)`** — a block comment
  in `config.h` shows the planned structure: section-name helper, INI iteration,
  fallback to defaults. Un-comment and connect when ready.
- **Commented-out trigger indices** — `case 10` / `case 12` in
  `CachedButtons::refreshCache()` are placeholders for analog triggers.
- **N64 joystick axis debug code** — commented out in `drawGamepadButtons()`.

### What still needs to change

1. **`resetButtonMap()`** — currently hardcodes SNES indices. Needs a `layout`
   parameter to populate the correct INI section.
2. **`SNESMapDefaults`** — should be one of several per-layout default structs
   (e.g. `N64MapDefaults`, `GCMapDefaults`).
3. **`loadButtonsFromConfig()`** in `PadCast.cpp` — currently reads raw INI
   via `getIni()`. Should migrate to a dedicated Config method.
4. **`GamepadTextures`** — needs a per-layout texture set mechanism.
5. **`CachedButtons`** — needs additional fields for analog axes and the extra
   buttons N64/GC controllers have.
6. **`RemapButtonScreens()`** — the 12-step walkthrough is SNES-only. Needs
   to be parameterized by button count and labels.

### Proposed implementation order (for v0.3.0)

1. Add `ControllerLayout` enum and section-name helper to Config.
2. Generalize `ButtonMap` / `CachedButtons` for variable button count.
3. Write `loadButtonMapping(layout)` on Config, replacing `getIni()` access.
4. Add N64 button textures and a `ControllerProfile` for texture selection.
5. Implement joystick rendering in `drawGamepadButtons()`.
6. Update `RemapButtonScreens()` to accept a button list.

---

### What exists for extension:
- Commented-out `case 10` and `case 12` in `CachedButtons::refreshCache()` for
  `GAMEPAD_BUTTON_LEFT_TRIGGER_2` / `GAMEPAD_BUTTON_RIGHT_TRIGGER_2`
- Commented-out N64 joystick debug code in `drawGamepadButtons()` reading axes
- `GAMEPAD_BUTTON_LEFT_THUMB` / `GAMEPAD_BUTTON_RIGHT_THUMB` constants exist in
  raylib but aren't referenced


## Refactoring Targets

### High Priority (for new controller support)

1. **Abstract controller types** — replace hardcoded SNES textures/buttons with
   a `ControllerProfile` or similar abstraction containing texture paths, button
   layout, and axis mappings

2. **ButtonMap generalization** — extend from 12 SNES buttons to support
   variable button counts per controller type

3. **Axis/joystick rendering** — add `drawJoystick()` or analog input rendering
   (needed for N64 stick, GC joystick, GC analog triggers)

4. ~~**validateConfig()** — template refactor to eliminate ~400 lines of repetition~~ ✅ DONE

### Medium Priority

5. **`resetButtonMap()` needs layout-awareness refactor** — currently hardcodes
   SNES button indices in `config.cpp`. When adding N64/GameCube layouts, this
   should accept a layout parameter and populate the appropriate INI section
   (e.g. `ButtonMap_N64`, `ButtonMap_GC`). Default values should come from
   per-layout structs, not repeated inline.

6. **Static locals in RemapButtonScreens()** — these persist across calls and
   complicate state management. Could be refactored into a class or struct

7. ~~**getDefault()** — giant if/else ladder, could use a map or template approach~~ ✅ DONE (static `unordered_map`)

8. **Caching system** — replace hand-rolled cache invalidation with a more
   structured approach (or verify it's worth keeping given raylib's performance)

### Low Priority

9. **Debug mode commented code** — N64 axis-reading code is commented out in
   production code (in `drawGamepadButtons()`)

10. ~~**Font static cache leak** — replaced static locals with file-scope cache
    and validity flag and `InvalidateFontCache()`, wired into Reload Config File.~~ ✅ DONE

### Completed This Session

11. ~~**`validateConfig()`** — ~400 lines → template-driven, ~38 lines.~~ ✅
12. ~~**`getDefault()`** — 82-line if/else ladder → static `unordered_map` lookup.~~ ✅
13. ~~**Duplicate save flags** — `needsSave` (local) + `mIsDirty` (member) consolidated into single `mNeedsSave`.~~ ✅
14. ~~**Boilerplate setters** — 9 individual blocks replaced by `setValue()` template.~~ ✅
15. ~~**Raw string button keys** — `updateButtonConfig()` now takes `ButtonConfigKey` enum.~~ ✅

---

## Build & Test Notes

- **Presets:** `linux-release`, `linux-debug`, `windows-release`, `windows-debug`
- **Compiler support:** GCC 11+, Clang 14+, MSVC 2022+ (C++23)
- **Ninja** is the recommended build system (presets use Ninja)
- Debug builds show console/log window; Release builds don't
- First build is slow due to FetchContent compiling raylib + raylib-cpp from source
- The `.cache/clangd/` directory contains clangd index files — useful for IDE navigation

---

## Key Files Cheat Sheet

| Need To... | Open This File |
|---|---|
| Understand the frame loop | `PadCast/src/main.cpp` |
| Add a new controller type | `PadCast/include/PadCast.h` (GamepadTextures, CachedButtons) + `PadCast/src/PadCast.cpp` |
| Add a config setting | `PadCast/include/config.h` (DefaultValues + getters/setters) + `config.cpp` (validateConfig) |
| Add a menu option | `PadCast/include/menus.h` (Menu enum) + `menus.cpp` (new Setup function) |
| Change resource paths | `PadCast/include/pathmanager.h` |
| Add packaging format | Top-level `CMakeLists.txt` (CPack) or `flatpak/` |
| Understand roadmap | `ROADWAY.md`, `TODO.md` |
