# PadCast Development Roadmap

This document outlines the planned development path for PadCast leading up to the version 1.0.0 stable release.

---

### **`v0.2.0`** - Current Release

This is the baseline version from which the roadmap begins.

---

### **`v0.2.5`** - Performance Update

This is a patch release focused entirely on internal improvements to enhance performance and efficiency without adding new user-facing features.

- [ ] Enable VSync support with a menu option.
- [ ] Test using event waiting to reduce idle CPU usage.
- [ ] Optimize rendering by drawing the base controller from a cached texture.
- [ ] Refactor data structures to be more cache-friendly.
- [ ] Measure impact of the `-ffast-math` CMake flag.

---

### **`v0.3.0`** - N64 Controller Support

This minor release introduces the first new controller type.

- [ ] Add support for N64 controller visualization.

---

### **`v0.4.0`** - GameCube Controller Support

This minor release adds support for another classic controller.

- [ ] Add support for GameCube controller visualization.

---

### **`v0.5.0`** - **`v0.8.0`** - Additional Input Support

This series of minor releases will expand the core functionality to include modern controllers and keyboard layouts. Each will be released individually.

- [ ] **v0.5.0**: Add NES controller support.
- [ ] **v0.6.0**: Add Xbox controller support.
- [ ] **v0.7.0**: Add PlayStation controller support.
- [ ] **v0.8.0**: Add WASD keyboard layout support.

---

### **`v0.9.0`** - Dual Controller Display

The final major feature addition before the 1.0.0 release, enhancing the application's utility.

- [ ] Implement the logic and UI to display two controllers on screen simultaneously.

---

### **`v1.0.0`** - First Stable Release 🚀

The official major release. This version signifies that the application is feature-complete according to the initial roadmap and is considered stable.

- [ ] Includes all previously added controller and layout support.
- [ ] Contains all performance improvements from the `v0.2.x` series.
- [ ] Features dual controller display capability.
- [ ] Remove any "beta" or "pre-release" labels from the application.
