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

#include <RtAudio.h>
#include <iostream>
#include <cmath>
#include <memory>
#include <numbers>

// Audio parameters
const unsigned int SAMPLE_RATE = 44100;
const unsigned int CHANNELS = 2;     // Stereo output
unsigned int BUFFER_FRAMES = 256;
const double FREQUENCY = 100.0;      // 160 Hz sine wave
const float AMPLITUDE = 0.5f;        // 50% amplitude to avoid clipping

// Global variables for sine wave generation
double phase = 0.0;
const double TWO_PI = 2.0 * std::numbers::pi;
const double PHASE_INCREMENT = TWO_PI * FREQUENCY / SAMPLE_RATE;

// Callback function for audio processing
int audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                 double /*streamTime*/, RtAudioStreamStatus status, void* /*userData*/) {
    if (status) {
        std::cerr << "Stream underflow detected!" << std::endl;
    }
    
    // Cast output buffer to float pointer
    float* buffer = static_cast<float*>(outputBuffer);
    
    // Generate sine wave
    for (unsigned int i = 0; i < nBufferFrames; i++) {
        float sample = AMPLITUDE * static_cast<float>(sin(phase));
        // Write to both channels (stereo)
        buffer[i * CHANNELS] = sample;        // Left channel
        buffer[i * CHANNELS + 1] = sample;    // Right channel
        
        // Update phase
        phase += PHASE_INCREMENT;
        if (phase >= TWO_PI) {
            phase -= TWO_PI;
        }
    }
    
    return 0;
}

int main() {
    // Create RtAudio instance
    std::unique_ptr<RtAudio> audio;
    try {
        audio = std::make_unique<RtAudio>();
    } catch (RtAudioErrorType& e) {
        // std::cerr << "Failed to create RtAudio instance:\n" << e.getMessage() << std::endl;
        return 1;
    }
    
    // Check available audio devices
    unsigned int devices = audio->getDeviceCount();
    if (devices < 1) {
        std::cerr << "No audio devices found!" << std::endl;
        return 1;
    }
    
    // Set output parameters
    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio->getDefaultOutputDevice();
    parameters.nChannels = CHANNELS;
    parameters.firstChannel = 0;
    
    // Set stream options
    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_SCHEDULE_REALTIME;
    
    try {
        // Open and start the stream
        audio->openStream(&parameters, nullptr, RTAUDIO_FLOAT32,
                         SAMPLE_RATE, &BUFFER_FRAMES, &audioCallback,
                         nullptr, &options);
        audio->startStream();
        
        std::cout << "Playing 160Hz sine wave. Press Enter to quit..." << std::endl;
        std::cin.get();
        
        // Stop and close the stream
        if (audio->isStreamRunning()) {
            audio->stopStream();
        }
        if (audio->isStreamOpen()) {
            audio->closeStream();
        }
    } catch (RtAudioErrorType& e) {
        // std::cerr << "Error:\n" << e.getMessage() << std::endl;
        return 1;
    }
    
    return 0;
}