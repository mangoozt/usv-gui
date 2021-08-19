#ifndef USV_GUI_APP_H
#define USV_GUI_APP_H

#include "usvdata/UsvRun.h"
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <nanogui/window.h>

class MyScreen;
class ScrollableSlider;
class IgnorantTextBox;
namespace nanogui {
    class Button;
}

class App {
    MyScreen* const screen;
    ScrollableSlider* slider{};
    nanogui::Button* run_usv_button{};
    GLFWwindow* window{};

    //ui
    IgnorantTextBox* time_label{};
    nanogui::Window* w_settings{};
    std::unique_ptr<USV::USVRunner> usv_runner{};
public:
    explicit App(GLFWwindow* glfw_window);

    ~App();

    void run();

    void run_case();

private:
    void initialize_gui();

    void load_directory(const std::string& data_directory);

    void update_time(double time);

    void reload();

    void open();

    void select_usv_executable();

    void show_settings();

    void update_slider_width(int width, int height);

    static void CursorPosCallback(GLFWwindow* window, double x, double y);

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int modifiers);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void CharCallback(GLFWwindow* window, unsigned int codepoint);

    static void DropCallback(GLFWwindow* window, int count, const char** filenames);

    static void WindowFocusCallback(GLFWwindow* window, int focused);

    static void ScrollCallback(GLFWwindow* window, double x, double y);

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};


#endif //USV_GUI_APP_H
