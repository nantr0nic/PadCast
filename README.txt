PadCast v0.2.0

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

-   Can change FPS to 30, 60, 90, 120 (or set custom FPS in config.ini)
-   Can change background color. Can set custom background color in config.ini file.
-   Can change color of the button presses. They can be tinted from the menu or a
    custom tint color can be set in config.ini file.
-   Can remap buttons.
-   Can use custom images
-   Persistent settings (resolution, colors, button maps, etc.)

-------------------------------------------------------------------------------------

2.  Requirements

-   OS: Windows 10+ or a modern Linux distribution that supports Arch/Fedora/Flatpak
    packages.
-   Graphics: OpenGL 3.3 support
-   Input: a SNES-style gamepad -- non-SNES-style controllers will work but only the
    D-pad and face buttons will register. Wireless gamepads work with the Windows
    build but are currently untested with Linux builds.

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

The following settings can be accessed and changed via the in-program menu: -
    ==> Resolution <==
    > The window can be manually resized by dragging the window borders, but
there are resolution presets available in the menu. If you require specific
dimensions, it can be manually set in config.ini 

    ==> Target FPS of the program (e.g. 30, 60, 90, 120). <==
    > This should usually be set to 60, but it can be changed to
match the FPS of the game/emulator you are running. If you require a specific target
FPS, it can be manually set in config.ini

    ==> Background Color <==
    > Generic background colors can be set in the menu. You can set a custom
background color in config.ini (see below)

    ==> Button Tint Color <==
    > This changes the color of the shapes that appear when a button is pressed. 
Generic colors can be set in the menu, but you can set a custom tint color in 
config.ini (see below)

    ==> Remap Buttons <==
    > Clicking "Start Remap" will begin a remap sequence where the program will go
through a series of prompts to register correct button pushes. The button map will
be saved in config.ini and will be used each time you load PadCast. 
"Reset to Default" will reset the button map to the SNES defaults the program came
with.

    ==> Load Controller <==
    > This will bring up the 4 gamepad inputs the program can detect. If your 
controller isn't detected by default, use this to select the controller you wish to
use. If you have more than one controller connected, you can use this menu to select
the controller you wish to use. If no inputs are detected, the menu will print 
"No Gamepads Detected"
    
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

 ==> ButtonMap and Debug <==

The in-program button remap should be sufficient, but for some reason if it is not
(please let me know!) you can enable "debug mode":

    [Debug]
    MODE=1

This will print the index number of the button you're pressing in the upper-left
corner of the screen. You can then set that number to the appropriate button under
\[ButtonMap\].

-------------------------------------------------------------------------------------

6. Custom Images

The images for the base controller and button pushes can be modified.
> On Windows, the images will be in the extracted folder under \resources\images\
> On Fedora/Arch, the images should be under /usr/share/padcast/resources/images/
> If using the Flatpak version, the images should be under: 
    - System install:
/var/lib/flatpak/app/com.github.nantr0nic.PadCast/current/active/files/share/padcast/resources/
    - User install:
\~/.local/share/flatpak/app/com.github.nantr0nic.PadCast/current/active/files/share/padcast/resources/

An .svg file is available in the GitHub repo. This includes the base controller and
pressed button images as layers. If using Inkscape, export each modified layer as its
own .png file. The file names must match. Meaning, the .png for A must be named A.png
and be located as /resources/images/pressed/A.png. Reach out to me if you have any
questions.

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

7. Building from Source

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

8. This is a work in progress!

-   I'd love to hear from you if you have any thoughts, comments, suggestions, etc.!
-   For now this is meant for USB SNES controllers, but it should work with any
    similar controller with the same or fewer number of buttons. My goal is for
    future versions to support NES, N64, GameCube, XBox, and Playstation controllers.
-   Previous versions of this program would be (falsely) flagged by Windows Defender
    as a trojan but I've since fixed the way this program is compiled and it has (so
    far...) not been flagged. It is not a trojan. If your Defender DOES flag it, let
    me know so I can continue troubleshooting it.

-------------------------------------------------------------------------------------

9. License

This project is licensed under the BSD 3-Clause License. See the LICENSE file for
details.

-------------------------------------------------------------------------------------

10. Credits

Developed by nantr0nic, with input from dank_meats.

This project uses raylib, raylib-cpp, and mINI -- thank you for making this project
possible!
