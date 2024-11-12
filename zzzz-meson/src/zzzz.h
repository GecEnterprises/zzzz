#include <boost/signals2/signal.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/circular_buffer.hpp>


struct ZzzzState {
    boost::atomic<float> audioFrame;

    boost::atomic<float> frequency;

    // Signal for start/stop audio actions
    boost::signals2::signal<void()> onAudioStartStop; // A simple signal, no parameters

    // Signal for GUI-triggered app close
    boost::signals2::signal<void()> onAppClose; // Simple signal, no parameters
};
