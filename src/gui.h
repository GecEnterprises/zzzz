void render_ui_thread();



struct ZzzzState;

boost::thread launch_gui_thread(std::shared_ptr<ZzzzState> signals);
