#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h> // or your preferred windowing backend
#include <boost/thread/thread.hpp>
#include <boost/circular_buffer.hpp>
#include <iostream>

#include "zzzz.h"

class GuiRunner{
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

private:
    std::shared_ptr<ZzzzState> state;
    float frame = 0.0;

public:
    GuiRunner(std::shared_ptr<ZzzzState> signals) {
        state = signals;
    }

    void render_ui_thread() {
        std::cout << "hi";

        if (!glfwInit())
            return;

        glfwSetErrorCallback(glfw_error_callback);

        std::cout << "creating context";
    
        // Initialize ImGui (this should be done once)
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
        if (window == nullptr)
            return;
        glfwMakeContextCurrent(window);

        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init("#version 130");

        ImGui_ImplGlfw_InitForOpenGL(window, true);

        boost::circular_buffer<float> samples = boost::circular_buffer<float>(100);


        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            
            // Start a new ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render your UI here (e.g., sliders, buttons to control PortAudio)
            ImGui::Text("Audio Control");

            samples.push_back(state->audioFrame.load(boost::memory_order_relaxed));

            float buffedSamples[100] = {0};
            for (size_t i = 0; i < 100 && i < samples.size(); ++i) {
                buffedSamples[i] = samples[i];
            }

            float frequency = state->frequency.load();
            ImGui::SliderFloat("float", &frequency, 60.0f, 500.0f);
            state->frequency.store(frequency);

            ImGui::PlotLines("Samples", buffedSamples, 100);

            if (ImGui::Button("Start Audio Stream")) {
                // Send a command to the audio thread to start or stop audio streaming
                // audio_thread_should_exit = false;
            }
            
            ImGui::Render();
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
};

boost::thread launch_gui_thread(std::shared_ptr<ZzzzState> signals) {
    GuiRunner guiRunner(signals);

    return boost::thread(&GuiRunner::render_ui_thread, guiRunner);
}