#include "SettingsWindow.h"
#include <nanogui/colorpicker.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/textbox.h>
#include <nanogui/tabwidget.h>

namespace {
    nanogui::Color toColor(const glm::vec4& color) {
        return {color.r, color.g, color.b, color.a};
    }

    glm::vec4 toVec4(const nanogui::Color& color) {
        return {color.r(), color.g(), color.b(), color.a()};
    }

    template<typename TMember>
    void add_color_picker(nanogui::Widget* widget, const std::string& label, TMember member, OGLWidget* map) {
        using namespace nanogui;
        new Label(widget, label, "sans-bold");
        auto cp = new ColorPicker(widget);
        cp->set_color(toColor(map->getAppearanceSettings().*member));

        cp->set_fixed_size({100, 20});
        cp->set_callback([map, member](const Color& c)
                         {
                             auto settings = map->getAppearanceSettings();
                             settings.*member = toVec4(c);
                             map->updateAppearanceSettings(settings);
                         });
    }

    template<typename TMember, typename Enum>
    void add_color_picker(nanogui::Widget* w, const std::string& label, TMember member, Enum anEnum, OGLWidget* map) {
        using namespace nanogui;
        new Label(w, label, "sans-bold");
        auto cp = new ColorPicker(w);
        cp->set_color(toColor((map->getAppearanceSettings().*member)[static_cast<size_t>(anEnum)]));

        cp->set_fixed_size({100, 20});
        cp->set_callback([map, member, anEnum](const Color& c)
                         {
                             auto settings = map->getAppearanceSettings();
                             (settings.*member)[static_cast<size_t>(anEnum)] = toVec4(c);
                             map->updateAppearanceSettings(settings);
                         });
    }

    template<typename TMember, typename E>
    void add_color_picker_pos(nanogui::Widget* w, const std::string& label, TMember member, E anEnum, OGLWidget* map) {
        using namespace nanogui;
        new Label(w, label, "sans-bold");
        auto cp = new ColorPicker(w);
        cp->set_color(toColor((map->getAppearanceSettings().*member)[static_cast<size_t>(anEnum)]));

        cp->set_fixed_size({100, 20});
        cp->set_callback([map, member, anEnum](const Color& c)
                         {
                             auto settings = map->getAppearanceSettings();
                             (settings.*member)[static_cast<size_t>(anEnum)] = toVec4(c);
                             map->updateAppearanceSettings(settings);
                             map->updatePositions();
                         });
    }
}

SettingsWindow::SettingsWindow(nanogui::Widget* parent, OGLWidget* map_widget, const std::string& title) :
        nanogui::Window(parent, title), map(map_widget) {
    using namespace nanogui;
    set_layout(new BoxLayout(nanogui::Orientation::Vertical));
    auto* tab_widget = this->add<TabWidget>();
    tab_widget->set_callback([](size_t) {});
    using AS = OGLWidget::AppearanceSettings;

    // Paths
    {
        auto* tab = new Widget(tab_widget);
        tab_widget->append_tab("Paths", tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        tab->set_layout(layout);

        auto& map_settings = map->getAppearanceSettings();

        add_color_picker(tab, "Target real maneuver:", &AS::path_colors, USV::PathType::TargetRealManeuver, map);
        add_color_picker(tab, "Target maneuver:", &AS::path_colors, USV::PathType::TargetManeuver, map);
        add_color_picker(tab, "Ship route:", &AS::path_colors, USV::PathType::Route, map);
        add_color_picker(tab, "Ship maneuver:", &AS::path_colors, USV::PathType::ShipManeuver, map);
    }
    // Ships
    /*
    {
        auto* tab = new Widget(tab_widget);
        tab_widget->append_tab("Ships", tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        tab->set_layout(layout);

        auto& map_settings = map->getAppearanceSettings();

        add_color_picker_pos(tab, "Target dangerous:", &AS::vessels_colors, Vessel::Type::TargetDangerous, map);
        add_color_picker_pos(tab, "Target real maneuver:", &AS::vessels_colors, Vessel::Type::TargetPotentiallyDangerous, map);
        add_color_picker_pos(tab, "Target not dangerous:", &AS::vessels_colors, Vessel::Type::TargetNotDangerous, map);
        add_color_picker_pos(tab, "Target undefined:", &AS::vessels_colors, Vessel::Type::TargetUndefined, map);
        add_color_picker_pos(tab, "Target on real maneuver:", &AS::vessels_colors, Vessel::Type::TargetOnRealManeuver, map);
        add_color_picker_pos(tab, "Ship on route:", &AS::vessels_colors, Vessel::Type::ShipOnRoute, map);
        add_color_picker_pos(tab, "Ship on maneuver:", &AS::vessels_colors, Vessel::Type::ShipOnManeuver, map);
    }
    */
    // sea
    {
        auto* tab = new Widget(tab_widget);
        tab_widget->append_tab("Sea", tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        tab->set_layout(layout);

        auto& map_settings = map->getAppearanceSettings();

        add_color_picker(tab, "Sea ambient:", &AS::sea_ambient, map);
        add_color_picker(tab, "Sea diffuse:", &AS::sea_diffuse, map);
        add_color_picker(tab, "Sea specular", &AS::sea_specular, map);
        {
            new Label(tab, "Sea shininess:", "sans-bold");
            auto intBox = new FloatBox<float>(tab);
            intBox->set_editable(true);
            intBox->set_fixed_size(Vector2i(100, 20));
            intBox->set_value(map_settings.sea_shininess);
            intBox->set_font_size(16);
            intBox->set_spinnable(true);
            intBox->set_min_value(1);
            intBox->set_max_value(1024);
            intBox->set_value_increment(2);
            intBox->set_callback([this](const float s)
                                 {
                                     auto settings = map->getAppearanceSettings();
                                     settings.sea_shininess = s;
                                     map->updateAppearanceSettings(settings);
                                 });
        }
    }

}
