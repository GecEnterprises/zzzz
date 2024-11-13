#include <boost/thread/thread.hpp>
#include <boost/circular_buffer.hpp>

#include "../zzzz.h"

class GuiRunner{
private:
    std::shared_ptr<ZzzzState> state;
    float frame = 0.0;
public:
    GuiRunner(std::shared_ptr<ZzzzState> signals) {
        state = signals;
    }

    void render_ui_thread() {
    
    }
};

boost::thread launch_gui_thread(std::shared_ptr<ZzzzState> signals) {
    GuiRunner guiRunner(signals);

    return boost::thread(&GuiRunner::render_ui_thread, guiRunner);
}