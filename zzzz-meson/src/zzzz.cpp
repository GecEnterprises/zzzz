#include <stdio.h>
#include <stdbool.h>
#include "simpleaudio.h"
#include "gui.h"

#include <boost/thread/thread.hpp>

#include "zzzz.h"

int main() {
    std::shared_ptr<ZzzzState> audioGuiSignals = std::make_shared<ZzzzState>();

    auto audioThread = launch_audio_thread(audioGuiSignals);
    launch_gui_thread(audioGuiSignals).join();

    audioThread.interrupt();

    return 0;
}