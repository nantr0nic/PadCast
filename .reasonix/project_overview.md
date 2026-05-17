# PadCast Project Overview

> A lightweight gamepad visualization tool for streamers. Displays a real-time
> overlay of controller button presses on screen.
>
> **Current version:** v0.2.5 | **License:** BSD 3-Clause

---

## What It Does

PadCast renders a controller image in a resizable window. When the user presses
a button on their gamepad, a tinted overlay shape appears on the corresponding
button's position on screen. Streamers capture this window via OBS or similar
software to show their inputs to viewers.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│                     main.cpp                         │
│  ● Window setup (raylib)                             │
│  ● Main loop (poll input → draw → swap buffers)      │
│  ● Cached frame-to-frame state                       │
└──────┬──────────────────────────────────────────────┘
       │ owns
┌──────▼──────────────────────────────────────────────┐
│                   PadCast (class)                    │
│  ● Owns GamepadTextures (all PNGs loaded as textures)│
│  ● Owns ButtonMap + CachedButtons (remap support)    │
│  ● drawGamepadButtons() — per-frame button drawing   │
│  ● Background color caching (getBGColor)             │
│  ● Tint color caching (drawGamepadButtons internal)  │
│  ● Gamepad connection stability logic                │
└──────┬──────────────────────────────────────────────┘
       │ reads
┌──────▼──────────────────────────────────────────────┐
│                   Config (class)                     │
│  ● Reads/writes config.ini via mINI library          │
│  ● Provides typed getters for every setting          │
│  ● Validates all values on load                      │
│  ● Setters update INI structure in memory            │
│  ● saveConfig() writes to disk                       │
└──────┬──────────────────────────────────────────────┘
       │ renders
┌──────▼──────────────────────────────────────────────┐
│                  Menu system                         │
│  ● Right-click / Space / M to open                   │
│  ● Stack of Menu enum states (Main→Video→Resolution) │
│  ● Each SetupXxxMenu() rebuilds menu.items vector    │
│  ● Keyboard (W/S, arrows) and mouse navigation       │
│  ● Lambda-based actions mutate Config + PadCast      │
└─────────────────────────────────────────────────────┘
```

---

## Directory Layout

```
PadCast/
├── PadCast/                        # Main application source
│   ├── CMakeLists.txt              # Sub-project build config
│   ├── include/                    # Headers
│   │   ├── PadCast.h               # Core class + textures/cache structs
│   │   ├── config.h                # Config class + default values
│   │   ├── menus.h                 # Menu system (enum, structs, setup funcs)
│   │   ├── debounce.h              # DebounceTimer helper class
│   │   ├── pathmanager.h           # Exec path / resource path resolution
│   │   └── mini/ini.h              # mINI library (vendored)
│   ├── src/                        # Implementation files
│   │   ├── main.cpp                # Entry point, main loop
│   │   ├── PadCast.cpp             # Core drawing/input logic
│   │   ├── config.cpp              # Config load/validate/save
│   │   ├── menus.cpp               # All menu definitions & input handling
│   │   └── winmain_proxy.cpp       # Windows WinMain → main bridge
│   ├── resources/                  # Default PNG assets
│   │   ├── images/                 # 960x540 base resolution
│   │   │   ├── controller.png      # Base controller image
│   │   │   └── pressed/            # Per-button overlay PNGs
│   │   │       ├── A.png, B.png, X.png, Y.png
│   │   │       ├── up.png, down.png, left.png, right.png
│   │   │       ├── L-bumper.png, R-bumper.png
│   │   │       ├── select.png, start.png
│   │   └── 1280x720_images/        # Higher-res alternative set
│   └── config/config.ini           # Default config template
├── controllerSVG/SNES/             # SVG source for controller art
├── flatpak/                        # Flatpak build files
├── screenshots/                    # Demo GIFs
├── CMakeLists.txt                  # Top-level CMake
├── CMakePresets.json               # Build presets (debug/release)
├── README.md                       # User-facing documentation
├── ROADMAP.md                      # Planned features
├── TODO.md                         # Development tasks
└── CHANGELOG.md                    # Release history
```

---

## Key Dependencies

| Library | Version | Role |
|---|---|---|
| **raylib** | 5.5 | Windowing, input, rendering, textures |
| **raylib-cpp** | 5.5.0 | C++ wrapper around raylib |
| **mINI** | (vendored) | Minimal INI file parser for config |
| **GLFW** | (raylib bundled) | Window/input backend (via raylib) |
| **SDL_GameControllerDB** | (raylib bundled) | Controller mapping database |

All fetched automatically by CMake's FetchContent (except for Flatpak builds
where they're separate modules).

---

## Main Loop Flow (main.cpp)

```
1. Create Config → loads/validates config.ini
2. Create raylib::Window (resizable)
3. Set icon from padcast.png
4. Configure VSync or target FPS
5. Create PadCast instance (loads button maps, caches)
6. Sleep 500ms (let controller connections stabilize)
7. Main loop:
   a. Check window resize → update Config
   b. BeginDrawing → ClearBackground
   c. Compute scaling (aspect-ratio-preserving)
   d. HandleMenuInput (open/close, navigate)
   e. Draw base controller texture
   f. Every 15 frames, check gamepad connection
   g. If connected + not remapping → drawGamepadButtons()
   h. Else → drawNoGamepadMessage()
   i. If RemapButtons menu active → RemapButtonScreens()
   j. If menu active → DrawMenu()
   k. EndDrawing
8. Save window size to config, saveConfig(), exit
```

---

## Configuration System

Config is stored in `config.ini` with these sections:

| Section | Purpose | Key Settings |
|---|---|---|
| `[Window]` | Window + display | INITIAL_WINDOW_WIDTH/HEIGHT, TARGET_FPS, USE_VSYNC, BG colors |
| `[Image]` | Image layout + tint | IMAGE_CANVAS_WIDTH/HEIGHT, tint palette/custom RGB |
| `[Gamepad]` | Controller selection | GAMEPAD_INDEX, STABILITY_THRESHOLD |
| `[Font]` | Text rendering | DEFAULT_FONT_SIZE, MIN_FONT_SIZE, TEXT_OFFSET |
| `[ButtonMap]` | Button remapping | DPAD_UP, A_BUTTON, START, etc. (12 keys) |
| `[Debug]` | Debug mode toggle | MODE (0 or 1) |

**Config file location (auto-detected):**
- **Portable mode** (Windows): `./config/config.ini` next to the exe
- **Linux installed:** `~/.config/padcast/config.ini`
- **Flatpak:** `~/.var/app/com.github.nantr0nic.PadCast/config/padcast/config.ini`

If `config.ini` doesn't exist, one is created with defaults. Values are
validated on every load (range-checked).

---

## Controller Support (Current: SNES)

PadCast currently maps **12 SNES buttons** to overlay images:

| Button | raylib Constant | Config Key | Default Index |
|---|---|---|---|
| D-Pad Up | GAMEPAD_BUTTON_LEFT_FACE_UP | DPAD_UP | 1 |
| D-Pad Right | GAMEPAD_BUTTON_LEFT_FACE_RIGHT | DPAD_RIGHT | 2 |
| D-Pad Down | GAMEPAD_BUTTON_LEFT_FACE_DOWN | DPAD_DOWN | 3 |
| D-Pad Left | GAMEPAD_BUTTON_LEFT_FACE_LEFT | DPAD_LEFT | 4 |
| X | GAMEPAD_BUTTON_RIGHT_FACE_UP | X_BUTTON | 5 |
| A | GAMEPAD_BUTTON_RIGHT_FACE_RIGHT | A_BUTTON | 6 |
| B | GAMEPAD_BUTTON_RIGHT_FACE_DOWN | B_BUTTON | 7 |
| Y | GAMEPAD_BUTTON_RIGHT_FACE_LEFT | Y_BUTTON | 8 |
| L (Left Shoulder) | GAMEPAD_BUTTON_LEFT_TRIGGER_1 | L_BUTTON | 9 |
| R (Right Shoulder) | GAMEPAD_BUTTON_RIGHT_TRIGGER_1 | R_BUTTON | 11 |
| Select | GAMEPAD_BUTTON_MIDDLE_LEFT | SELECT | 13 |
| Start | GAMEPAD_BUTTON_MIDDLE_RIGHT | START | 15 |

The `ButtonMap` struct stores an `unordered_map<int, int>` mapping raylib button
constants → "display index" (the number raylib reports when that button is
pressed). `CachedButtons` flattens these into direct int members for fast
lookup in the draw loop.

---

## Image System

- **Base controller** is drawn from a single `controller.png` texture.
- **Pressed overlays** are individual PNGs in `images/pressed/`.
- All PNGs must have transparent backgrounds.
- Button shapes should be **white** — they get tinted at draw time.
- Two resolution sets: 960x540 (default) and 1280x720.
- Canvas dimensions are configurable via `IMAGE_CANVAS_WIDTH/HEIGHT`.
- **Scaling** is computed every frame: uniform scale preserving aspect ratio
  with centered offsets.

---

## Menu System

- Activated by right-click, Space, or M.
- `MenuContext` holds the active `Menu` enum value, item list, and selection.
- Each submenu has a `SetupXxxMenu()` function that rebuilds the item vector
  with lambdas for actions.
- Menu hierarchy:
  ```
  Main
  ├── Video
  │   ├── Resolution (1280x720, 960x540, 640x360, 480x270)
  │   ├── Target FPS (30/60/90/120, disabled when VSync on)
  │   └── Toggle VSync
  ├── Visuals
  │   ├── Background Color (Black/White/Red/Green/Blue/Custom)
  │   └── Image Tint (White/Red/Green/Blue/Custom)
  └── Controller
      ├── Select Gamepad (lists available controllers)
      └── Remap Buttons (guided 12-step sequence)
  ```
- **Button remapping** walks through all 12 buttons sequentially, waiting for
  input with a 500ms debounce. Press Escape to cancel.

---

## Cross-Platform Details

### Windows
- `winmain_proxy.cpp` bridges WinMain → main
- MSVC-specific flags in CMakeLists.txt for anti-virus compatibility
  (`/guard:cf`, `/GL`, `/LTCG`, dynamic CRT)
- `.app.manifest` and `version.rc` for Windows metadata

### Linux
- XDG Base Directory for config (`~/.config/padcast/`)
- System data directory: `/usr/share/padcast/`
- Flatpak support with `com.github.nantr0nic.PadCast.yml`
- RPM packaging via CPack
- Desktop entry + metainfo.xml for app stores
- Icon installed to `/usr/share/icons/hicolor/48x48/apps/`

### Both
- Portable mode detection: if `resources/` or `config/` dir exists next to the
  exe, paths are resolved relative to the executable directory.
- Fallback: system-wide install paths.

---

## Build System

```bash
# Release (recommended)
cmake --preset=linux-release
cmake --build --preset=linux-release

# Debug (shows console/log window)
cmake --preset=linux-debug
cmake --build --preset=linux-debug

# Manual
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Dependencies are fetched automatically via CMake's FetchContent (except flatpak
builds which use the YAML manifest).

---

## Current State & Roadmap

- **v0.2.5** — current release ("Under the Hood" update)
- **Next planned:** v0.3.0 (N64 Controller Support)
- Remote branch `feature/refactor-gamepad` exists with unmerged work
- Currently on the `develop` branch
- The `ButtonMap` and `CachedButtons` structures have commented-out sections
  for future trigger/joystick support (N64, GameCube)

---

## Known Issues

- Some controllers mislabeled by OS/GLFW (e.g., USB GameCube shown as
  "XBox Controller")
- Wireless gamepads untested on Linux
- Codebase is currently **SNES-hardcoded** — `GamepadTextures` loads SNES-
  specific PNGs, `CachedButtons` only has SNES button fields
- The `validateConfig()` method in config.cpp is extremely repetitive
  (lots of copy-paste per key) — marked with a TODO to rewrite as a template
- Debug mode logs button/axis data to raylib's trace log and stdout
