# To Do...
### Next
- [ ] Enable VSync support. Provide option to enable in menu.
- [ ] Test using event waiting to see if it lowers CPU usage.
- [ ] Draw base controller from cached texture. Measure impact.
- [ ] See if there's data that can be structured to be more cache-friendly.
- [ ] Measure impact of CMake flag "-ffast-math" on performance. (May be negligible.)
- [ ] Test using SDL2 backend for better gamepad compatibility.
- [ ] Restructure code to begin supporting next controllers.

### Then
- [ ] N64 Controller Support
- [ ] GameCube Controller Support

### Finally
- [ ] Work on XBox, PS, NES, and WASD-layout support.
- [ ] Add support for simultaneously displaying two controllers.

### Non-short and non-medium term:
>- [X] Try to provide a transparent background option.
>> Got the transparency to work on Windows but it is finnicky. Might be a compatibility
issue between raylib and what it can hook into in Windows. Setting aside for now and
will come back to later.

#### Completed (from first to last)
- [X] Resolution saves between open/close
- [X] Use .ini file for configuration
- [X] Stop log window from opening
- [X] Option for changing FPS (60 / 30)
- [X] Get it to NOT be flagged as a trojan!
- [X] Add ability to remap buttons.
- [X] Resolution menu
- [X] Add pressed button tint color options
- [X] Use a main.cpp file and separate out PadCast.cpp features.
- [X] Refactor. Clean up. Simplify. Make style consistent.
- [X] Get current version (v0.10) to work on Linux. (Released as v0.2.0)
- [X] Adjust image canvas resolution for better scaling.