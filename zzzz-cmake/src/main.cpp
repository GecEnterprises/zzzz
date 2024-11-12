#ifdef _WIN32
#include "gui/windows.h"
#else
#include "gui/linux.h"
#endif

int main(int, char**) {
    #ifdef _WIN32
    launch_windows_gui();
    #else
    launch_linux_gui();
    #endif
}