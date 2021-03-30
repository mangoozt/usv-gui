#ifndef USV_GUI_APP_H
#define USV_GUI_APP_H

#include <GLFW/glfw3.h>
#include <iostream>
#include "MyScreen.h"
#include "usvdata/InputUtils.h"

class App {
    MyScreen* screen{};
    Slider* slider{};
    GLFWwindow* window{};

    //ui
    TextBox* time_label{};
public:
    explicit App(GLFWwindow* glfw_window);

    void run();

    void initialize_gui();

private:
    void load_directory(const std::string& data_directory);

    void update_time(double time);

    void reload();

    void open();

    static void CursorPosCallback(GLFWwindow* window, double x, double y);

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int modifiers);

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void CharCallback(GLFWwindow* window, unsigned int codepoint);

    static void DropCallback(GLFWwindow* window, int count, const char** filenames);

    static void ScrollCallback(GLFWwindow* window, double x, double y);

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};


#endif //USV_GUI_APP_H
