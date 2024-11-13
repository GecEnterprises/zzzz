#include <boost/thread/thread.hpp>

struct ZzzzState;

boost::thread launch_gui_thread(std::shared_ptr<ZzzzState> signals);