#include "path_appearance.h"

namespace 
{
const glm::vec4 initial_colors[] = 
{
    glm::vec4(0.724168, 0.63479, 0, 1),    //  TargetManeuver
    glm::vec4(0.7f, 0.7f, 0.5f, 1),    //  TargetRealManeuver
    glm::vec4(0.769072533, 0.0, 1, 1), //  ShipManeuver
    glm::vec4(0, 0.600625, 1, 1)       //  Route
};
}

glm::vec4 PathAppearance::getColor(const USV::Path &path) const {
    return path_colors[static_cast<size_t>(path.getType())];
}

glm::vec4 PathAppearance::getColor(USV::PathType type) const {
    return path_colors[static_cast<size_t>(type)];
}

bool PathAppearance::isVisible(const USV::Path &path) const {
    return path_visability[static_cast<size_t>(path.getType())];
}

bool PathAppearance::isVisible(USV::PathType type) const {
    return path_visability[static_cast<size_t>(type)];
}

PathAppearanceNotifier::PathAppearanceNotifier() 
    : StateNotifier<PathAppearance>({})
{
    static_assert(sizeof(glm::vec4) * static_cast<size_t>(USV::PathType::End) == sizeof(initial_colors), 
        "initial_colors size mismatches number of path types");
    std::memcpy(value.path_colors, initial_colors, sizeof(initial_colors));

    for (auto& el : value.path_visability) {
        el = true;
    }
}

void PathAppearanceNotifier::updateColor(USV::PathType type,
                                         const glm::vec4 &color) {
    value.path_colors[static_cast<size_t>(type)] = color;
    notify();
}

void PathAppearanceNotifier::setVisible(USV::PathType type, bool visibility) {
    value.path_visability[static_cast<size_t>(type)] = visibility;
    notify();
}