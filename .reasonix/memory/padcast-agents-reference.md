<!-- priority: low -->
<!-- description: AGENTS.md full body minus work preferences — project info, conventions, architecture, common tasks, gotchas, memory system -->

# PadCast — AGENT Reference (Project Context)

Extracted from AGENTS.md. Work preferences stored separately.

## Project
- Gamepad visualization tool for streamers — shows real-time controller button presses as an on-screen overlay (captured by OBS etc.)
- **Language:** C++23 (GCC 11+, Clang 14+, MSVC 2022+)
- **Libraries:** raylib 5.5, raylib-cpp 5.5.0, mINI (vendored in include/mini/)
- **Cross-platform:** Windows 10+, Linux (Flatpak, RPM), OpenGL 3.3
- **Current version:** v0.2.5 — currently SNES-only controller support
- **Branch:** `develop` (active), `main` (releases)
- There's a `feature/refactor-gamepad` branch on origin with unmerged work

## Conventions
- **American English** in all docs, comments, and code identifiers
- Tab indentation (existing code uses tabs, ~4-space width)
- C++23 standard
- No single-character variable names — even in lambdas. Use at least three characters (`val` not `v`, `idx` not `i`). Exception: loop induction variables in one-liner `for` or `while` headers are okay.
- raylib-cpp wrapper preferred over raw raylib C API where possible
- `m_` prefix for class member variables (`mConfig`, `mTextures`)
- SCREAMING_SNAKE for config key names and constexpr defaults
- Error messages via `std::cerr`, info/debug via `std::cout`
- Guard headers with `#ifndef` / `#define` / `#endif` (no `#pragma once`)

## Architecture TL;DR

```
main.cpp  →  owns main loop, creates:
             ├── Config          (config.ini read/write via mINI)
             ├── PadCast         (textures, button maps, drawing)
             └── MenuContext     (enum-driven menu system with lambda actions)
```

- **Config** — lazy-loaded from INI file, validated on every load. Config file location auto-detects portable mode (exe-relative) vs system install (XDG).
- **PadCast** — owns GamepadTextures (13 PNGs), ButtonMap + CachedButtons, handles per-frame drawing and connection stability.
- **Menus** — right-click/Space/M opens; each submenu has a Setup function that rebuilds a vector of MenuItem structs (label + lambda action).
- **Scaling** — aspect-ratio-preserving uniform scale computed every frame.
- **Caching** — hand-rolled lazy cache with invalidation flags for BG color and tint color (avoids re-reading config INI every frame).

## Common tasks
- **Add a config key:** 1 line in validateConfig() template + a getter/setter in config.h + a constexpr default in DefaultValues
- **Add a menu option:** new Menu enum value + SetupXxxMenu() function in menus.h/cpp + wiring in HandleMenuInput
- **Add a controller type:** not yet abstracted — currently SNES-hardcoded in GamepadTextures, CachedButtons, and drawGamepadButtons()

## Gotchas & risk areas
- `RemapButtonScreens()` uses **static locals** for state — fragile, resets don't always work correctly on subsequent calls
- `getDefault()` in config.cpp was refactored from an if/else ladder to a `static std::unordered_map`
- `validateConfig()` was ~400 lines of repetitive if/else blocks → now template-driven via `validateInt()` at ~38 lines
- Config path resolution differs between portable mode (exe-relative) and system install (`~/.config/padcast/` or XDG) — test both when changing paths
- Font size caching uses `static` locals in DrawMenu() / RemapButtonScreens() — won't pick up config changes until restart
- MSVC builds have special anti-virus flags (`/guard:cf`, `/GL`, `/LTCG`) that affect link times and debugger behavior
- Flatpak builds use separate dependency modules (not FetchContent) — test the flatpak YAML if adding new dependencies

## Memory system
- Project facts saved to `.reasonix/memory/global/` as markdown
- These auto-load into the system prompt on next session start
- Key reference files in `.reasonix/`: `AGENTS.md`, `project_overview.md`, `codebase_analysis.md`, `session_state.md`
- Session state exported to `session_state.md` at the end of each session so new sessions pick up context without replaying history
