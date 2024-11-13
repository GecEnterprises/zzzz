//  #ifdef _WIN32
//  #include "gui/windows.h"
//  #else
//  #include "gui/linux.h"
//  #endif

//  int main(int, char**) {
//      #ifdef _WIN32
//      launch_windows_gui();
//      #else
//      launch_linux_gui();
//      #endif
//  }


#include <stdio.h>
#include <stdbool.h>
#include <boost/thread/thread.hpp>

#include "zzzz.h"
#include "audio/simpleaudio.h"

int main() {
    std::shared_ptr<ZzzzState> audioGuiSignals = std::make_shared<ZzzzState>();

    launch_audio_thread(audioGuiSignals).join();
    // launch_gui_thread(audioGuiSignals).join();

    // audioThread.interrupt();

    return 0;
}