#ifndef USV_GUI_IGNORANTTEXTBOX_H
#define USV_GUI_IGNORANTTEXTBOX_H

#include <nanogui/textbox.h>

using namespace nanogui;

class IgnorantTextBox : public TextBox {
public:
    inline explicit IgnorantTextBox(Widget* parent) : TextBox(parent) {};

    inline bool keyboard_character_event(unsigned int codepoint) override {
        return false;
    }
};

#endif //USV_GUI_IGNORANTTEXTBOX_H
