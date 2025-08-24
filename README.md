# PadCast
### A simple gamepad display that shows controller activity. Geared towards streamers.
---
![PadCast v0.03](screenshots/padcast-current.gif)

Work in progress! So far supports (any?) controller that matches the configuration
you see in the image above.

Written in C++ and is light on resources.

---

Current features:
- Persistent resolution.
- Can change FPS to 30, 60, 90, 120.
- Can change background color. Can set custom background color in config/config.ini file.
- Can change color of the button presses. They can be tinted from the menu or a custom tint color
can be set in config/config.ini file.
- Can remap buttons.

---

The images are .png files under /resources/ and can be edited to change the appearance of
the controller in the program. If you do edit the .png files, be sure to keep the canvas size 1280x720.
Or, if you DO want to modify the png's to be a different resolution (change the canvas size), 
then modify the image_canvas_width and image_canvas_height values in config.ini file to match the canvas size of your own png's.

---

### This is pre-release! (Warning)

This version of PadCast has mitigations to avoid false positive flags from Windows Defender. 
It has been tested on several machines and those machines failed to produce the false positive. 
If you do encounter a false positive flag from Windows Defender, please let me know either on the GitHub page or by email. 
This will help me keep track of the false positive issue -- thank you!

---

This project uses [raylib](https://github.com/raysan5/raylib), [raylib-cpp](https://github.com/RobLoach/raylib-cpp), 
and [mINI](https://github.com/metayeti/mINI) -- thank you for making this project possible!