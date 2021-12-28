#ifndef VESSEL_APPEARENCE_H
#define VESSEL_APPEARENCE_H

#include "state_notifier.h"
#include "glvessels.h"
#include <glm/glm.hpp>

class VesselAppearance {
public:
    glm::vec4 vessel_colors[static_cast<size_t>(Vessel::Type::End)];
public:
    glm::vec4 getColor(Vessel::Type type) const;
};

class VesselAppearanceNotifier : public StateNotifier<VesselAppearance> {
public:
    VesselAppearanceNotifier();
    void updateColor(Vessel::Type type, const glm::vec4 &color);
};

#endif // VESSEL_APPEARENCE_H