#ifndef USV_GUI_SETTINGSWINDOW_H
#define USV_GUI_SETTINGSWINDOW_H


#include <nanogui/window.h>

class SettingsWindow : public nanogui::Window {
public:
    explicit SettingsWindow(Widget *parent, const std::string &title = "Settings");
};


#endif //USV_GUI_SETTINGSWINDOW_H
