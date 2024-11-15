

#include <stdio.h>
#include <stdbool.h>
#include <vector>
#include <boost/thread/thread.hpp>
#include "zzzz.h"
#include "audio/simpleaudio.h"

int main() {
    std::shared_ptr<ZzzzState> audioGuiSignals = std::make_shared<ZzzzState>();
    launch_audio_thread(audioGuiSignals).join();   
 
    return 0;
}