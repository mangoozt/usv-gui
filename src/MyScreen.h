#ifndef USV_GUI_MYSCREEN_H
#define USV_GUI_MYSCREEN_H

#include <nanogui/nanogui.h>

using namespace nanogui;

class OGLWidget;

class MyScreen : public Screen {
    bool lbutton_down{false};
    bool mbutton_down{false};
    bool wait_callback{false};
    OGLWidget* map_ = nullptr;
public:

    MyScreen();

    void draw_contents() override;

    ~MyScreen() override;

    void scroll_callback(double x, double y);

    void key_callback(int key, int scancode, int action, int mods);

    void mouse_button_callback(GLFWwindow* w, int button, int action, int modifiers);

    void cursor_pos_callback(double x, double y);

    inline OGLWidget& map() const { return *map_; }
};


#endif //USV_GUI_MYSCREEN_H
