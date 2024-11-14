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

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

#include <stdio.h>
#include <stdbool.h>
#include <boost/thread/thread.hpp>

#include "zzzz.h"
#include "audio/simpleaudio.h"

int main() {
    std::shared_ptr<ZzzzState> audioGuiSignals = std::make_shared<ZzzzState>();

    // launch_audio_thread(audioGuiSignals).join();
    // launch_gui_thread(audioGuiSignals).join();

    // audioThread.interrupt();

    static mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    FILE *file = fopen("/home/station/Downloads/gec.mp3", "rb");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // Read and decode MP3 frames
    mp3dec_frame_info_t info;
    std::vector<mp3d_sample_t> pcm(1152 * 2 * 4); // 1152 samples per frame, 2 channels

    while (true) {
        std::vector<uint8_t> buffer(4096);
        size_t bytesRead = fread(buffer.data(), 1, buffer.size(), file);
        if (bytesRead == 0) break;

        std::vector<mp3d_sample_t> pcm(1152 * 2 * 4);

        int samples = mp3dec_decode_frame(&mp3d, buffer.data(), bytesRead, pcm.data(), &info);
        fprintf(stdout, "written %d samples | bytes read %d\n", samples, bytesRead);

        for (int i = 0; i < samples; i++) {
            if (pcm[i] != 0) {
                printf("%d\n", pcm[i]);
            }
        }
    }

    fclose(file);

    return 0;
}