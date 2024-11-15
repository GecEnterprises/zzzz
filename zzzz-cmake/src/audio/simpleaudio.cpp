#include <RtAudio.h>
#include <iostream>
#include <cmath>
#include <memory>
#include <numbers>

#include "../zzzz.h"

#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>

#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>

// Audio parameters
const unsigned int SAMPLE_RATE = 44100;
const unsigned int CHANNELS = 2;     // Stereo output
unsigned int BUFFER_FRAMES = 256;
const double FREQUENCY = 100.1;      // 160 Hz sine wave
const float AMPLITUDE = 0.5f;        // 50% amplitude to avoid clipping

// Global variables for sine wave generation
double phase = 0.0;
const double TWO_PI = 2.0 * std::numbers::pi;
const double PHASE_INCREMENT = TWO_PI * FREQUENCY / SAMPLE_RATE;

std::vector<mp3d_sample_t> all_pcm_data;
int seek = 0;

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
        buffer[i * CHANNELS] = all_pcm_data[seek];        // Left channel
        seek+=1;

        buffer[i * CHANNELS + 1] = all_pcm_data[seek];    // Right channel

        seek+=1;
    }

    // fprintf(stdout, "%d\n", seek);

    return 0;
}

int exec() {
     static mp3dec_t mp3d;
    mp3dec_init(&mp3d);
    
      #ifdef _WIN32
    // Windows
    char exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    char *lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *lastSlash = '\0';
    char filePath[MAX_PATH];
    snprintf(filePath, sizeof(filePath), "%s\\resources\\kesokku.mp3", exePath);
#else
    // Linux/Unix
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
    if (len != -1) {
        exePath[len] = '\0';
        char *lastSlash = strrchr(exePath, '/');
        if (lastSlash) *lastSlash = '\0';
    }
    char filePath[PATH_MAX];
    snprintf(filePath, sizeof(filePath), "%s/resources/kesokku.mp3", exePath);
#endif

FILE *file = fopen(filePath, "rb");
if (!file) {
    fprintf(stderr, "Error opening file\n");
    return 1;
}
    
    // Buffer for reading raw MP3 data
    std::vector<uint8_t> mp3_buffer;
    const size_t READ_CHUNK_SIZE = 16 * 1024; // 16KB chunks
    mp3_buffer.reserve(READ_CHUNK_SIZE);
    
    // Buffer for decoded PCM data
    
    // Accumulated PCM data for all decoded frames
    all_pcm_data.reserve(10000000); // Reserve space to avoid frequent reallocations
    
    mp3dec_frame_info_t info;
    size_t buffer_offset = 0;
    
    while (true) {

    std::vector<mp3d_sample_t> pcm_buffer(MINIMP3_MAX_SAMPLES_PER_FRAME);
        // Read more data if buffer is running low
        if (buffer_offset >= mp3_buffer.size()) {
            mp3_buffer.resize(READ_CHUNK_SIZE);
            size_t bytes_read = fread(mp3_buffer.data(), 1, READ_CHUNK_SIZE, file);
            
            if (bytes_read == 0) {
                break; // End of file
            }
            
            mp3_buffer.resize(bytes_read);
            buffer_offset = 0;
        }
        
        // Decode next frame
        int samples = mp3dec_decode_frame(
            &mp3d, 
            mp3_buffer.data() + buffer_offset,
            mp3_buffer.size() - buffer_offset,
            pcm_buffer.data(),
            &info
        );
        
        if (samples > 0) {
            // Store decoded PCM data
            all_pcm_data.insert(
                all_pcm_data.end(),
                pcm_buffer.begin(),
                pcm_buffer.end()
            );
            
            fprintf(stdout, "Decoded frame: %d bytes, %d samples\n", 
                    info.frame_bytes, samples);
        }
        
        if (info.frame_bytes > 0) {
            buffer_offset += info.frame_bytes;
        } else {
            // No valid frame found, skip one byte and try again
            buffer_offset++;
        }
    }
    
    
    fprintf(stdout, "Total decoded samples: %zu\n", all_pcm_data.size());
    
    fclose(file);


    // Create RtAudio instance
    std::unique_ptr<RtAudio> audio;
    try {
        audio = std::make_unique<RtAudio>();
    }
    catch (RtAudioErrorType& e) {
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

        while (true) {
            std::cin.get();
            seek = rand() % all_pcm_data.size();
            fprintf(stdout, "seek at: %d", seek);
        }

        // Stop and close the stream
        if (audio->isStreamRunning()) {
            audio->stopStream();
        }
        if (audio->isStreamOpen()) {
            audio->closeStream();
        }
    }
    catch (RtAudioErrorType& e) {
        // std::cerr << "Error:\n" << e.getMessage() << std::endl;
        return 1;
    }

    return 0;
}

boost::thread launch_audio_thread(std::shared_ptr<ZzzzState> signals) {
    return boost::thread(exec);
}