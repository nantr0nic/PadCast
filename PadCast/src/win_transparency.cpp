#include <windows.h>
#include "win_transparency.h"

void SetWindowTransparency(void* windowHandle) {
    HWND hwnd = (HWND)windowHandle;
    SetWindowLong(hwnd, GWL_EXSTYLE,
        GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
}