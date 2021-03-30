#include "MyScreen.h"
#include <GLFW/glfw3.h>

void MyScreen::draw_contents() {
    clear();
    nvgBeginFrame(m_nvg_context, static_cast<float>(m_size[0]), static_cast<float>(m_size[1]), m_pixel_ratio);
    map_->paintGL(m_nvg_context);
    nvgEndFrame(m_nvg_context);
}

void MyScreen::scroll_callback(double x, double y) {
    Screen::scroll_callback_event(x, y);
    // pointer not on GUI
    if (!m_redraw) {
        map_->scroll(x, y);
        m_redraw = true;
    }
}

void MyScreen::key_callback(int key, int scancode, int action, int mods) {
    Screen::key_callback_event(key, scancode, action, mods);
    // pointer not on GUI
    if (!m_redraw) {
        map_->keyPress(key);
        m_redraw = true;
    }
}

void MyScreen::mouse_button_callback(GLFWwindow* w, int button, int action, int modifiers) {
    wait_callback = true;
    Screen::mouse_button_callback_event(button, action, modifiers);
    wait_callback = false;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action)
            lbutton_down = true;
        else if (GLFW_RELEASE == action)
            lbutton_down = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (GLFW_PRESS == action)
            mbutton_down = true;
        else if (GLFW_RELEASE == action)
            mbutton_down = false;
    }
    // pointer not on GUI
    if (!m_redraw) {
        double xpos, ypos;
        glfwGetCursorPos(w, &xpos, &ypos);
        if (action == GLFW_PRESS) {
            map_->mousePressEvent(xpos, ypos, button, modifiers);
        }
        m_redraw = map_->uniforms_dirty();
    }
}

void MyScreen::cursor_pos_callback(double x, double y) {
    Screen::cursor_pos_callback_event(x, y);
    if (!m_redraw && !m_drag_active && !wait_callback) {
        map_->mouseMoveEvent(x, y, lbutton_down, mbutton_down);
        m_redraw = map_->uniforms_dirty();
    }
}
