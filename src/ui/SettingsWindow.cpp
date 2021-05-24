#include "SettingsWindow.h"
#include <nanogui/colorpicker.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/textbox.h>
#include <nanogui/combobox.h>
#include <nanogui/checkbox.h>
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
    GridLayout *layout = new GridLayout(Orientation::Horizontal, 2,
                                        Alignment::Middle, 15, 5);
    layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
    layout->set_spacing(0, 10);
    this->set_layout(layout);

//    /* FP widget */ {
//        new Label(this, "Floating point :", "sans-bold");
//        textBox = new TextBox(this);
//        textBox->set_Editable(true);
//        textBox->set_FixedSize(Vector2i(100, 20));
//        textBox->set_Value("50");
//        textBox->set_Units("GiB");
//        textBox->set_DefaultValue("0.0");
//        textBox->set_FontSize(16);
//        textBox->set_Format("[-]?[0-9]*\\.?[0-9]+");
//    }

    /* Positive integer widget */ {
        new Label(this, "Positive integer :", "sans-bold");
        auto intBox = new IntBox<int>(this);
        intBox->set_editable(true);
        intBox->set_fixed_size(Vector2i(100, 20));
        intBox->set_value(50);
        intBox->set_units("Mhz");
        intBox->set_default_value("0");
        intBox->set_font_size(16);
        intBox->set_format("[1-9][0-9]*");
        intBox->set_spinnable(true);
        intBox->set_min_value(1);
        intBox->set_value_increment(2);
    }

//    /* Checkbox widget */ {
//        new Label(this, "Checkbox :", "sans-bold");
//
//        cb = new CheckBox(this, "Check me");
//        cb->set_Font_Size(16);
//        cb->set_Checked(true);
//    }

    auto &map_settings = map->getAppearanceSettings();

    new Label(this, "Combo box :", "sans-bold");
    auto *cobo = new ComboBox(this, {"Item 1", "Item 2", "Item 3"});
    cobo->set_font_size(16);
    cobo->set_fixed_size(Vector2i(100, 20));

    new Label(this, "Sea ambient :", "sans-bold");
    auto cp_sea_ambient = new ColorPicker(this);
    cp_sea_ambient->set_color(toColor(map_settings.sea_ambient));

    cp_sea_ambient->set_fixed_size({100, 20});
    cp_sea_ambient->set_final_callback([map](const Color &c) {
        auto settings = map->getAppearanceSettings();
        settings.sea_ambient = toVec4(c);
        map->updateAppearanceSettings(settings);
    });

    new Label(this, "Sea ambient :", "sans-bold");
    auto cp_sea_specular = new ColorPicker(this, {255, 120, 0, 255});
    cp_sea_specular->set_fixed_size({100, 20});
    cp_sea_specular->set_final_callback([](const Color &c) {
        std::cout << "ColorPicker Final Callback: ["
                  << c.r() << ", "
                  << c.g() << ", "
                  << c.b() << ", "
                  << c.w() << "]" << std::endl;
    });

}
