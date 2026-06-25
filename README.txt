PadCast v0.3.0

A lightweight gamepad visualization tool that shows controller activity.
Geared towards streamers.

Table of Contents 
- 1 Features 
- 2 Requirements 
- 3 Usage 
- 4 Installation 
    - 4.1 Windows 
    - 4.2 Linux 
- 5 Configuration 
    - 5.1 In-program Menu 
    - 5.2 Config.ini 
- 6 Custom Images 
- 7 Building from Source 
- 8 This is a work in progress! 
- 9 License 
- 10 Credits

-------------------------------------------------------------------------------------

1.  Features

-   Can set FPS (30, 60, 90, 120 or custom value in config.ini) or use VSync.
-   Can change background color. Can set custom background color in config.ini file.
-   Can change color of the button presses.
-   Can switch between controller layouts (SNES, N64).
-   Can remap buttons and joystick.
-   N64 joystick display with configurable idle/moving draw modes.
-   N64 C-buttons and Z-trigger support.
-   Can use custom images (layout-aware directory structure).
-   Persistent settings (resolution, colors, button maps, layout choice, etc.)

-------------------------------------------------------------------------------------

2.  Requirements

-   OS: Windows 10+ or a modern Linux distribution that supports Arch/Fedora/Flatpak
    packages.
-   Graphics: OpenGL 3.3 support
-   Input: a SNES or N64 USB gamepad. Non-SNES/N64 controllers will work but may
    have limited button support depending on the adapter. Wireless gamepads work
    with the Windows build but are currently untested with Linux builds.

-------------------------------------------------------------------------------------

3.  Usage

Your USB gamepad should be automatically detected. If it is not, go to the "Load
Controller" option in the menu and select your USB gamepad there.

The menu can be accessed by mouse or keyboard: right-click in the window to
open/close the menu, or hit spacebar or "M". You can navigate the menu using the
mouse as expected, or use W/S and arrow-keys to navigate and Enter to select.

Hitting escape will exit the program.

-------------------------------------------------------------------------------------

4.  Installation

    4.1 - Windows

Download the .zip, extract it, and run PadCast.exe. That's it!

    4.2 - Linux

    -== Flatpak ==-

Download the flatpak from the releases page. Depending on your system's software,
you can click/double-click PadCast.flatpak to open and install it. If that fails,
you can install the flatpak in the terminal:

    # Install the flatpak
    flatpak install PadCast.flatpak

If you get an error that you are missing "org.freedesktop.Platform/x86_64/23.08"...

    # Add the flathub repository to your system
    flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

    # Install the 23.08 version of Platform
    flatpak install flathub org.freedesktop.Platform/x86_64/23.08

    # Now you can install PadCast
    flatpak install PadCast.flatpak

    -== RPM (Fedora) ==-

You can install the RPM package on Fedora and other RPM-based distributions. On
Fedora you can double-click the RPM to install it, or you can install it via
terminal:

    # Download the latest .rpm file from the releases page
    sudo dnf install ./padcast-0.1.0-1.x86_64.rpm

-------------------------------------------------------------------------------------

5. Configuration

    5.1 In-program Menu

The menu can be accessed by mouse or keyboard: right-click in the window to
open/close the menu, or hit spacebar or "M". You can navigate the menu using the
mouse as expected, or use W/S and arrow-keys to navigate and Enter to select.

The following settings can be accessed and changed via the in-program menu:

                    Video Settings
    ==> Resolution <==
    > The window can be manually resized by dragging the window borders, but
there are resolution presets available in the menu. If you require specific
dimensions, it can be manually set in config.ini 

    ==> Target FPS of the program (e.g. 30, 60, 90, 120). <==
    > This should usually be set to 60, but it can be changed to
match the FPS of the game/emulator you are running. If you require a specific target
FPS, it can be manually set in config.ini

    ==> Enable/Disable VSync <==
    > This will enable/disable VSync. While VSync is enabled, the Target FPS setting
will be ignored. This will enable/disable VSync. While VSync is enabled, the Target FPS 
setting will be ignored. If you enable VSync, the program will limit the FPS to match 
your monitor's refresh rate. If you change your monitor's refresh rate while the program 
is running, you will need to toggle VSync off and back on to match the new refresh rate.
Please note that VSync may increase CPU usage depending on your specific hardware and GPU 
driver settings; generally the higher refresh rate your monitor is set to, the more CPU 
usage.

                    Visuals Settings
    ==> Background Color <==
    > Generic background colors can be set in the menu. You can set a custom
background color in config.ini (see below)

    ==> Button Tint Color <==
    > This changes the color of the shapes that appear when a button is pressed. 
Generic colors can be set in the menu, but you can set a custom tint color in 
config.ini (see below)

                    Controller Settings
    ==> Load Controller <==
    > This will bring up the 4 gamepad inputs the program can detect. If your 
controller isn't detected by default, use this to select the controller you wish to
use. If you have more than one controller connected, you can use this menu to select
the controller you wish to use. If no inputs are detected, the menu will print 
"No Gamepads Detected"

    ==> Remap Buttons <==
    > Clicking "Start Remap" will begin a remap sequence where the program will go
through a series of prompts to register correct button pushes. Press Spacebar to
skip a button (keeps its current value). The button map is saved to the appropriate
layout section in config.ini (e.g., [SNES_ButtonMap] or [N64_ButtonMap]). "Reset to
Default" resets to the factory defaults for the current layout.

    ==> Layout <==
    > Switch between SNES and N64 controller layouts. Selecting N64 will load the
N64 controller image and button overlays. The layout choice is saved and restored
on startup.

    ==> Draw Joystick (N64 only) <==
    > Toggle between "Draw idle" (joystick shown at rest position) and "Draw when
moving" (only shown while the stick is being pushed). This setting is saved persistently.

    ==> Remap Joystick (N64 only) <==
    > Walks you through remapping the four cardinal joystick directions (Up, Right,
Down, Left). The program auto-discovers which analog axes your controller uses and
saves them to config.ini.

                    Other Settings  
    ==> Reload Config File <==
    > If you edit the config.ini file while the program is running, click "Reload
Config File" to enable the changes you've made (make sure to save config.ini first!).
For example, if you want to try out different custom background colors: select
"Custom Color" in the Background Color menu, edit the relevant custom color values in
config.ini, save the config.ini file, then click "Reload Config File" to see the new
custom color take effect.


    5.2 Config.ini

Changes you make with the in-program menu will be reflected in config.ini, but you
can also set your own custom values in config.ini. If you set incompatible values,
the program will reset them to default values. If you accidentally delete any values
or find you've modified the config to something crazy you'd like to undo, you can
simply delete config.ini and the program will regenerate the file using default
values.

The section and key names should not be changed (e.g. \[Window\], TARGET_FPS). If you
do change them, your changes will be ignored and the program will re-generate those
sections with default values.

Described below is how to modify some settings/values you might be interested in
setting yourself.

On Windows, the config.ini file will be in the extracted folder in .ini

If installed using a regular Linux package, the config file will be in
\~/.config/PadCast/config.ini

For flatpak installations, config.ini will be in
\~/.var/app/com.github.nantr0nic.PadCast/config/padcast/config.ini

 ==> Window Resolution <==

To set a custom Window resolution, set the desired dimensions here:

    INITIAL_WINDOW_WIDTH=640
    INITIAL_WINDOW_HEIGHT=360

 ==> Target FPS <==

To set a custom target FPS, set the desired frames per second (cannot exceed 250):

    TARGET_FPS=60

 ==> Background Color <==

To set a custom background color, set the desired RGB values in the following
sections of config.ini:

    CUSTOM_BG_RED=102
    CUSTOM_BG_GREEN=0
    CUSTOM_BG_BLUE=153

If you need help figuring out RGB values for a color you'd like, you can search
'color picker' on DuckDuckGo and use the built-in applet there.

 ==> Tint color <==

To set a custom tint color, set the desired RGB values in the following sections of
config.ini:

    IMAGE_TINT_RED=255
    IMAGE_TINT_GREEN=255
    IMAGE_TINT_BLUE=0

If you want to preview your custom background or tint colors, you can edit config.ini
while the program is running and click "Reload Config File" in the menu to load your
new values.

 ==> Font Sizes <==

You can change the font size of on-screen messages or the menu in \[Font\]:

    DEFAULT_FONT_SIZE=35

 ==> Layout-Aware Button Mapping <==

Button mappings are now stored per-layout under their own INI sections:

    [SNES_ButtonMap]     ; SNES controller buttons (12 keys)
    [N64_ButtonMap]      ; N64 controller buttons (14 keys: D-pad, A, B, C-buttons, L, R, Z, Start)

Each section has the same key names as before (DPAD_UP, A_BUTTON, etc.). The N64
section adds C_UP, C_DOWN, C_LEFT, C_RIGHT, and Z_BUTTON. "Reset to Default" resets
to per-layout factory defaults.

 ==> N64-Specific Config Keys <==

    [Gamepad]
    LAYOUT=0             ; 0=SNES, 1=N64 (set via the Layout menu)
    STICK_X_AXIS=0       ; Which analog axis drives left/right joystick movement
    STICK_Y_AXIS=1       ; Which analog axis drives up/down joystick movement

    [Image]
    DRAW_JOYSTICK_IDLE=1 ; 1=always show joystick, 0=only show while moving

    [Gamepad]
    STICK_DEADZONE=0.1   ; Joystick deadzone (0.0–0.5). Default 0.1 is usually fine.

The stick axis values are auto-discovered by the "Remap Joystick" menu option.
You only need to set these manually if your controller uses non-standard axis indices.

Why adjust the deadzone? If your joystick overlay jitters or drifts while the
stick is at rest, increase the deadzone (try 0.15 or 0.2). If small, precise
movements aren't registering, lower it (try 0.05). The deadzone filters out
tiny axis fluctuations so the joystick only moves when you intentionally push it.

 ==> ButtonMap and Debug <==

The in-program button remap should be sufficient, but for some reason if it is not
(please let me know!) you can enable "debug mode":

    [Debug]
    MODE=1

This will print the index number of the button you're pressing in the upper-left
corner of the screen. You can then set that number to the appropriate button under
[SNES_ButtonMap] or [N64_ButtonMap].

-------------------------------------------------------------------------------------

6. Custom Images

Images are now organized by controller layout:

    resources/
    +-- SNES/
    |   +-- images/
    |       +-- controller.png
    |       +-- pressed/   (A.png, B.png, X.png, Y.png, etc.)
    +-- N64/
    |   +-- images/
    |       +-- controller.png
    |       +-- pressed/   (A.png, B.png, C-up.png, Z.png, joystick.png, etc.)
    +-- padcast.png        (program icon)

> On Windows, these folders will be in the extracted folder under \resources\
> On Fedora/Arch, under /usr/share/padcast/resources/
> If using the Flatpak version:
    - System install: /var/lib/flatpak/app/com.github.nantr0nic.PadCast/current/active/files/share/padcast/resources/
    - User install: ~/.local/share/flatpak/app/com.github.nantr0nic.PadCast/current/active/files/share/padcast/resources/

SVG source files are available in the GitHub repo. This includes the base controller
and pressed button images as layers for SNES and N64. If using Inkscape, export each
modified layer as its own .png file. The file names must match the defaults. Reach out
to me if you have any questions.

The images must: - be .png files - use the same names and locations as the default
images - use a transparent background - button push shapes must match their
respective location on the base controller

The default image/canvas resolution is 960x540. If you change the resolution of the
images used by the program, update the following values in config.ini to match the
resolution of your modified .png's:

    IMAGE_CANVAS_WIDTH=960
    IMAGE_CANVAS_HEIGHT=540

If you want to scale above 960x540, there is a /resources/1280x720_images/ folder with
higher resolution .png files you can use for better scaling. If you want to use them,
rename the current /resources/images folder to something like ""960x540_images"" and then
rename the 1280x720_images folder to "images" for the program to use those instead.

If you modify the pressed-button images in /resources/images/pressed/, the tints will
still work as expected if you keep the shapes WHITE.


-------------------------------------------------------------------------------------

7. Known Issues

- Some controllers may be mislabeled by the OS or the underlying gamepad library
(GLFW/SDL_GameControllerDB). For example, a USB GameCube controller may be labeled
as "XBox Controller".
- N64 Start button may not be detected by some USB adapters. If your Start button
doesn't register after remapping, press Spacebar during the remap sequence to skip
it. The Start overlay will be disabled when its value is 0 (the default).
- C-buttons and Start sharing indices: On some adapters, C-buttons may share button
indices with standard controller inputs (e.g., C-Up and Start both using index 15).
The "Reset to Default" option restores the factory defaults which should resolve any
conflicts.

-------------------------------------------------------------------------------------

8. Building from Source

 -== Prerequisites ==-

-   C++23 compatible compiler (GCC 11+, Clang14+, or MSVC 2022+)
-   CMake 3.15+
-   Ninja (recommended for faster builds)

 -== CMake Presets using Clang/Ninja ==-

Using the provided CMake presets:

    git clone https://github.com/nantr0nic/PadCast.git
    cd PadCast

    # Build the release build (optimized for performance)
    cmake --preset=linux-release    # or windows-release on Windows
    cmake --build --preset=linux-release

    # Or, if you want to build a debug version (will open with debug info window)
    cmake --preset=linux-debug      # or windows-debug on Windows
    cmake --build --preset=linux-debug

 -== Alternative Method ==-

If you prefer not to use presets or don't have Clang/Ninja installed:

    git clone https://github.com/nantr0nic/PadCast.git
    cd PadCast
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
    cmake --build build

 -== Running ==-

After building, the executable will be located in: - Preset builds:
out/build/\[preset-name\]/PadCast/ - Manual builds: build/PadCast/

Notes: - Dependencies are automatically downloaded via CMake's FetchContent - First
build may take longer as dependencies are fetched and compiled - Ninja builds are
significantly faster for incremental rebuilds

-------------------------------------------------------------------------------------

9. This is a work in progress!

-   I'd love to hear from you if you have any thoughts, comments, suggestions, etc.!
-   SNES and N64 controllers are currently supported, with full joystick and
    C-button support for N64. Future versions aim to add GameCube controller
    support, and eventually Xbox and PlayStation controllers.
-   Previous versions of this program would be (falsely) flagged by Windows Defender
    as a trojan but I've since fixed the way this program is compiled and it has (so
    far...) not been flagged. It is not a trojan. If your Defender DOES flag it, let
    me know so I can continue troubleshooting it.

-------------------------------------------------------------------------------------

10. License

This project is licensed under the BSD 3-Clause License. See the LICENSE file for
details.

-------------------------------------------------------------------------------------

11. Credits

Developed by nantr0nic, with input from dank_meats.

This project uses raylib, raylib-cpp, and mINI -- thank you for making this project
possible!
