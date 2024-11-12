int audio_thread_function();

#include <boost/thread/thread.hpp>

struct ZzzzState;

boost::thread launch_audio_thread(std::shared_ptr<ZzzzState> signals);

#ifndef WAVE_DATA_H
#define WAVE_DATA_H

typedef struct {
    double phase;
    double phase_increment;
    std::shared_ptr<ZzzzState> signals;
} WaveData;

#endif // WAVE_DATA_H