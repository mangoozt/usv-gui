#include "SettingsWindow.h"
#include "../provider.h"
#include "../path_appearance.h"
#include "../sea_appearance.h"
#include "../vessel_appearance.h"
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

    void add_path_color_picker(nanogui::Widget* w, const std::string& label, USV::PathType type) {
        using namespace nanogui;
        new Label(w, label, "sans-bold");
        auto cp = new ColorPicker(w);

        auto& appearance = Provider<PathAppearanceNotifier>::of();

        cp->set_color(toColor(appearance.value.getColor(type)));

        cp->set_fixed_size({100, 20});
        cp->set_callback([&appearance, type](const Color& c)
                         {
                             appearance.updateColor(type, toVec4(c));
                         });
    }

    void add_vessel_color_picker(nanogui::Widget* w, const std::string& label, Vessel::Type type) {
        using namespace nanogui;
        new Label(w, label, "sans-bold");
        auto cp = new ColorPicker(w);

        auto& appearance = Provider<VesselAppearanceNotifier>::of();

        cp->set_color(toColor(appearance.value.getColor(type)));

        cp->set_fixed_size({100, 20});
        cp->set_callback([&appearance, type](const Color& c)
                         {
                             appearance.updateColor(type, toVec4(c));
                         });
    }

    template <typename TMember>
    void add_sea_color_picker(nanogui::Widget* w, const std::string& label, TMember member) {
                using namespace nanogui;
        new Label(w, label, "sans-bold");
        auto cp = new ColorPicker(w);

        auto& appearance = Provider<SeaAppearanceNotifier>::of();
        Material material = appearance.value.material;

        cp->set_color(toColor({material.*member, 1.0}));

        cp->set_fixed_size({100, 20});
        cp->set_callback([&appearance, member](const Color& c)
                         {
                             Material material = appearance.value.material;
                             material.*member = glm::vec3(toVec4(c));
                             appearance.updateMaterial(material);
                         });
    }
}

SettingsWindow::SettingsWindow(nanogui::Widget* parent, const std::string& title) :
        nanogui::Window(parent, title) {
    using namespace nanogui;
    set_layout(new BoxLayout(nanogui::Orientation::Vertical));
    auto* tab_widget = this->add<TabWidget>();
    tab_widget->set_callback([](size_t) {});

    // Paths
    {
        auto* tab = new Widget(tab_widget);
        tab_widget->append_tab("Paths", tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        tab->set_layout(layout);

        add_path_color_picker(tab, "Target maneuver:", USV::PathType::TargetManeuver);
        add_path_color_picker(tab, "Ship route:", USV::PathType::Route);
        add_path_color_picker(tab, "WastedManeuver:", USV::PathType::WastedManeuver);
        add_path_color_picker(tab, "Ship maneuver:", USV::PathType::ShipManeuver);
    }
    // Ships
    {
        auto* tab = new Widget(tab_widget);
        tab_widget->append_tab("Ships", tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        tab->set_layout(layout);

        add_vessel_color_picker(tab, "Target dangerous:", Vessel::Type::TargetDangerous);
        add_vessel_color_picker(tab, "Target real maneuver:", Vessel::Type::TargetPotentiallyDangerous);
        add_vessel_color_picker(tab, "Target not dangerous:", Vessel::Type::TargetNotDangerous);
        add_vessel_color_picker(tab, "Target undefined:", Vessel::Type::TargetUndefined);
        add_vessel_color_picker(tab, "Ship on route:", Vessel::Type::ShipOnRoute);
        add_vessel_color_picker(tab, "Ship on maneuver:", Vessel::Type::ShipOnManeuver);
    }
    // sea
    {
        auto* tab = new Widget(tab_widget);
        tab_widget->append_tab("Sea", tab);
        auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                      Alignment::Middle, 15, 5);
        layout->set_col_alignment({Alignment::Maximum, Alignment::Fill});
        layout->set_spacing(0, 10);
        tab->set_layout(layout);

        auto &appearance = Provider<SeaAppearanceNotifier>::of();
        Material material = appearance.value.material;

        add_sea_color_picker(tab, "Sea ambient:", &Material::ambient);
        add_sea_color_picker(tab, "Sea diffuse:", &Material::diffuse);
        add_sea_color_picker(tab, "Sea specular", &Material::specular);
        {
            new Label(tab, "Sea shininess:", "sans-bold");
            auto intBox = new FloatBox<float>(tab);
            intBox->set_editable(true);
            intBox->set_fixed_size(Vector2i(100, 20));
            intBox->set_value(material.shininess);
            intBox->set_font_size(16);
            intBox->set_spinnable(true);
            intBox->set_min_value(1);
            intBox->set_max_value(1024);
            intBox->set_value_increment(2);
            intBox->set_callback([&appearance](const float s)
                                 {
                                     Material material = appearance.value.material;
                                     material.shininess = s;
                                     appearance.updateMaterial(material);
                                 });
        }
    }

}
