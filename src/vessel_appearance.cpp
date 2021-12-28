#include "vessel_appearance.h"

namespace
{
const glm::vec4 initial_colors[] =
    {
        {0, 1, 0, 1},                         // TargetNotDangerous
        {1.0f, 0.6, 0.2, 1.0f},               // TargetPotentiallyDangerous
        {0.8, 0, 0, 1},                       // TargetDangerous
        {0, 0.426016808, 1, 1},               // TargetUndefined
        {0, 0.333551884, 1, 1},               // TargetOnRealManeuver
        {0.7, 0.7, 0.7, 1},                   // TargetInitPosition
        {0.8f, 0.8f, 0.8f, 1.0f},             // ShipOnRoute
        {0.66566503, 0.0, 0.738230228, 1.0f}, // ShipOnManeuver
        {0.4, 0.4, 1.0, 1.0f}                 // ShipInitPosition}
    };
}

glm::vec4 VesselAppearance::getColor(Vessel::Type type) const {
    return vessel_colors[static_cast<size_t>(type)];
}

VesselAppearanceNotifier::VesselAppearanceNotifier()
    : StateNotifier<VesselAppearance>({}) {
    static_assert(sizeof(glm::vec4) * static_cast<size_t>(Vessel::Type::End) == sizeof(initial_colors), 
        "initial_colors size mismatches number of vessel types");
    std::memcpy(value.vessel_colors, initial_colors, sizeof(initial_colors));
}

void VesselAppearanceNotifier::updateColor(Vessel::Type type,
                                           const glm::vec4 &color) {

    value.vessel_colors[static_cast<size_t>(type)] = color;
    notify();
}