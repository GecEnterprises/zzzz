#include <boost/signals2/signal.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/circular_buffer.hpp>


struct ZzzzState {
    boost::atomic<float> audioFrame;

    boost::atomic<float> frequency;

    boost::signals2::signal<void()> onAudioStartStop; // A simple signal, no parameters

    boost::signals2::signal<void()> onAppClose; // Simple signal, no parameters
};
