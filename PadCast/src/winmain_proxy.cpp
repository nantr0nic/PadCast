#if defined(_WIN32)

#include <windows.h>
extern int main(int argc, char** argv);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return main(__argc, __argv);
}
#endif