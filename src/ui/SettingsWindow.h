#ifndef USV_GUI_SETTINGSWINDOW_H
#define USV_GUI_SETTINGSWINDOW_H


#include <nanogui/window.h>
#include "../oglwidget.h"

class SettingsWindow : public nanogui::Window {
    OGLWidget *map;
public:
    explicit SettingsWindow(Widget *parent, OGLWidget *map, const std::string &title = "Settings");


};


#endif //USV_GUI_SETTINGSWINDOW_H
