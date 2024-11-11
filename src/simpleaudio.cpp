#include <portaudio.h>
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256
// #define FREQUENCY 70
#define AMPLITUDE 0.7

#include <math.h>
#include "simpleaudio.h"

#include <stdio.h>
#include <stdbool.h>

#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>
#include "zzzz.h"

class AudioRunner
{
    public:
    AudioRunner(std::shared_ptr<ZzzzState> signals) {
        audioGuiSignals = signals;
    }

    static int audioCallback(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData) {
        WaveData *data = (WaveData*)userData;
        float *out = (float*)outputBuffer;
        
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            float sample = (sin(data->phase) >= 0.0) ? AMPLITUDE : -AMPLITUDE;
            
            // Output to both channels (stereo)
            *out++ = sample;  // left
            *out++ = sample;  // right
            
            // Update phase
            auto freq = data->signals->frequency.load();
            data->phase_increment = 2.0 * M_PI * freq / SAMPLE_RATE;
            data->phase += data->phase_increment;
            if (data->phase >= 2.0 * M_PI) {
                data->phase -= 2.0 * M_PI;
            }

            data->signals->audioFrame.store(sample, boost::memory_order_relaxed);
        }
        
        return paContinue;
    }

    int audio_thread_function() {
        PaStream *stream;
        PaError err;
        WaveData data;
        
        audioGuiSignals->frequency.store(600);
        data.phase = 0.0;
        data.phase_increment = 0.0;
        data.signals = audioGuiSignals;
        
        err = Pa_Initialize();
        if (err != paNoError) {
            printf("PortAudio error: %s\n", Pa_GetErrorText(err));
            return 1;
        }
        
        err = Pa_OpenDefaultStream(&stream,
                                0,          // no input channels
                                2,          // stereo output
                                paFloat32,  // sample format
                                SAMPLE_RATE,
                                FRAMES_PER_BUFFER,
                                &AudioRunner::audioCallback,
                                &data);
        
        if (err != paNoError) {
            printf("Stream open error: %s\n", Pa_GetErrorText(err));
            Pa_Terminate();
            return 1;
        }
        
        err = Pa_StartStream(stream);
        if (err != paNoError) {
            printf("Stream start error: %s\n", Pa_GetErrorText(err));
            Pa_CloseStream(stream);
            Pa_Terminate();
            return 1;
        }
        
        printf("Playing 600Hz square wave. Press Enter to stop...\n");
        getchar();
        
        err = Pa_StopStream(stream);
        if (err != paNoError) {
            printf("Stream stop error: %s\n", Pa_GetErrorText(err));
        }
        
        Pa_CloseStream(stream);
        Pa_Terminate();
    }

    private:
    std::shared_ptr<ZzzzState> audioGuiSignals;
};

boost::thread launch_audio_thread(std::shared_ptr<ZzzzState> signals) {
    AudioRunner audioRunner(signals);

    return boost::thread(&AudioRunner::audio_thread_function, audioRunner);
}
