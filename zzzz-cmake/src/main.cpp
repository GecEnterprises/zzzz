// #ifdef _WIN32
// #include "gui/windows.h"
// #else
// #include "gui/linux.h"
// #endif

// int main(int, char**) {
//     #ifdef _WIN32
//     launch_windows_gui();
//     #else
//     launch_linux_gui();
//     #endif
// }

#include <portaudio.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <numbers>

class AudioGenerator {
private:
    static constexpr double SAMPLE_RATE = 44100;
    static constexpr double FREQUENCY = 66.0;  // 400Hz tone
    static constexpr double AMPLITUDE = 0.5;    // 50% volume
    double phase = 0.0;

    PaStream* stream = nullptr;

    // Callback function must be static to work with C API
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData) {
        auto* generator = static_cast<AudioGenerator*>(userData);
        auto* out = static_cast<float*>(outputBuffer);
        
        // Generate sine wave samples
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            out[i] = AMPLITUDE * std::sin(generator->phase);
            
            // Advance phase for next sample
            generator->phase += 2.0 * std::numbers::pi * FREQUENCY / SAMPLE_RATE;
            
            // Keep phase in [0, 2π]
            if (generator->phase >= 2.0 * std::numbers::pi) {
                generator->phase -= 2.0 * std::numbers::pi;
            }
        }
        
        return paContinue;
    }

public:
    AudioGenerator() {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            throw std::runtime_error(std::string("PortAudio initialization failed: ") + 
                                   Pa_GetErrorText(err));
        }

        // Open output stream
        err = Pa_OpenDefaultStream(
            &stream,
            0,          // no input channels
            1,          // mono output
            paFloat32,  // sample format
            SAMPLE_RATE,
            paFramesPerBufferUnspecified,  // let PortAudio choose buffer size
            paCallback,
            this  // pass this instance to callback
        );

        if (err != paNoError) {
            Pa_Terminate();
            throw std::runtime_error(std::string("Failed to open stream: ") + 
                                   Pa_GetErrorText(err));
        }
    }

    ~AudioGenerator() {
        if (stream) {
            Pa_CloseStream(stream);
        }
        Pa_Terminate();
    }

    void start() {
        PaError err = Pa_StartStream(stream);
        if (err != paNoError) {
            throw std::runtime_error(std::string("Failed to start stream: ") + 
                                   Pa_GetErrorText(err));
        }
        std::cout << "Playing 400Hz tone. Press Enter to stop..." << std::endl;
    }

    void stop() {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            throw std::runtime_error(std::string("Failed to stop stream: ") + 
                                   Pa_GetErrorText(err));
        }
    }

    bool isActive() const {
        return Pa_IsStreamActive(stream) == 1;
    }
};

int main() {
    try {
        AudioGenerator generator;
        generator.start();
        
        // Wait for Enter key
        std::cin.get();
        
        generator.stop();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}