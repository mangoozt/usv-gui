#include "SettingsWindow.h"
#include <nanogui/colorpicker.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/textbox.h>
#include <nanogui/tabwidget.h>
#include <iostream>

namespace {
    nanogui::Color toColor(const glm::vec4 &color) {
        return {color.r, color.g, color.b, color.a};
    }

    glm::vec4 toVec4(const nanogui::Color &color) {
        return {color.r(), color.g(), color.b(), color.a()};
    }
}

SettingsWindow::SettingsWindow(nanogui::Widget *parent, OGLWidget *map, const std::string &title) :
        nanogui::Window(parent, title), map(map) {
    using namespace nanogui;
    set_layout(new BoxLayout(nanogui::Orientation::Vertical));
    auto *tab_widget = this->add<TabWidget>();
    {
        auto* sea_tab = new Widget(tab_widget);
        tab_widget->append_tab("Sea", sea_tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        sea_tab->set_layout(layout);

        auto& map_settings = map->getAppearanceSettings();

        {
            new Label(sea_tab, "Sea ambient :", "sans-bold");
            auto cp_sea_ambient = new ColorPicker(sea_tab);
            cp_sea_ambient->set_color(toColor(map_settings.sea_ambient));

            cp_sea_ambient->set_fixed_size({100, 20});
            cp_sea_ambient->set_callback([map](const Color& c)
                                         {
                                             auto settings = map->getAppearanceSettings();
                                             settings.sea_ambient = toVec4(c);
                                             map->updateAppearanceSettings(settings);
                                         });
        }
        {
            new Label(sea_tab, "Sea diffuse:", "sans-bold");
            auto cp_sea_ambient = new ColorPicker(sea_tab);
            cp_sea_ambient->set_color(toColor(map_settings.sea_diffuse));

            cp_sea_ambient->set_fixed_size({100, 20});
            cp_sea_ambient->set_callback([map](const Color& c)
                                         {
                                             auto settings = map->getAppearanceSettings();
                                             settings.sea_diffuse = toVec4(c);
                                             map->updateAppearanceSettings(settings);
                                         });
        }
        {
            new Label(sea_tab, "Sea specular:", "sans-bold");
            auto cp_sea_ambient = new ColorPicker(sea_tab);
            cp_sea_ambient->set_color(toColor(map_settings.sea_diffuse));

            cp_sea_ambient->set_fixed_size({100, 20});
            cp_sea_ambient->set_callback([map](const Color& c)
                                         {
                                             auto settings = map->getAppearanceSettings();
                                             settings.sea_specular = toVec4(c);
                                             map->updateAppearanceSettings(settings);
                                         });
        }
        {
            new Label(sea_tab, "Sea shininess:", "sans-bold");
            auto intBox = new FloatBox<float>(sea_tab);
            intBox->set_editable(true);
            intBox->set_fixed_size(Vector2i(100, 20));
            intBox->set_value(map_settings.sea_shininess);
            intBox->set_font_size(16);
            intBox->set_spinnable(true);
            intBox->set_min_value(1);
            intBox->set_max_value(1024);
            intBox->set_value_increment(2);
            intBox->set_callback([map](const float s)
                                 {
                                     auto settings = map->getAppearanceSettings();
                                     settings.sea_shininess = s;
                                     map->updateAppearanceSettings(settings);
                                 });
        }
    }
}
