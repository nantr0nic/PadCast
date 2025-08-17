# To Do...
### Short term:
Short term goals are aimed to be achieved in the current state of only supporting SNES (or any/just a D-pad / 6 button gamepad).
- [ ] Add ability to remap buttons.
- [ ] Resolution menu

>#### Completed
- [X] Resolution saves between open/close
- [X] Use .ini file for configuration
- [X] Stop log window from opening
- [X] Option for changing FPS (60 / 30)
> It is now changeable through the .ini, but need to make configurable in GUI.
- [X] Get it to NOT be flagged as a trojan!
> Tested on several machines and complete *for now* -- it might be the case that
Windows Defender will produce a false positive with future definition updates.

### Medium term:
- [ ] N64 & Gamecube Support
> The PNG's are complete. Need to implement support.
- [ ] NES layouts and support.
- [ ] XBox & PS controller layouts / support
- [ ] Get it compiled on Linux and make features work (suppressing log window, etc.)

### Non-short and non-medium term:
- Explore supplementing raylib with SDL's gamepad library for increased compatibility
- Visual features (choose controller colors, etc.)
>- [X] Try to provide a transparent background option.
>> Got the transparency to work on Windows but it is finnicky. Might be a compatibility
issue between raylib and what it can hook into in Windows. Setting aside for now and
will come back to later.