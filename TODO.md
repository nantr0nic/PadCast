# To Do...
### Short term:
Short term goals are aimed to be achieved in the current state of only supporting SNES (or any/just a D-pad / 6 button gamepad).
- [X] Resolution saves between open/close
- [ ] Resolution menu
- [X] Use .ini file for configuration

- [X] Stop log window from opening
- [X] Option for changing FPS (60 / 30)
> It is now changeable through the .ini, but of course I'd like to make it a menu option as well :)
- [ ] !!! Get it to NOT be flagged as a trojan! !!!
- [ ] Add ability to remap buttons.

### Medium term:
- [ ] N64 & Gamecube Support
> The PNG's are complete. Need to implement support.
- [ ] NES layouts and support.
- [ ] XBox & PS controller layouts / support
- [ ] Get it compiled on Linux and make features work (suppressing log window, etc.)

### Non-short and non-medium term:
- Explore supplementing raylib with SDL's gamepad library for increased compatibility
- Visual features (choose controller colors, etc.)
>- [ ] Try to provide a transparent background option.
>> Got the transparency to work on Windows but it is finnicky. 

### Dev non-features:
- [ ] Refactor Config, remove validation repetitions
> - [ ] Streamline default value creation if config.ini absent
- [X] Setup a GitHub repo