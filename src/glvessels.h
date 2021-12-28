#ifndef USV_GUI_GLVESSELS_H
#define USV_GUI_GLVESSELS_H

#include "usvdata/Restrictions.h"
#include "usvdata/CaseData.h"
#include "state_notifier.h"
#include <glm/glm.hpp>
#include <utility>
#include <memory>
#include <nanovg.h>

class Program;

class Buffer;

struct Vessel {

    enum class Type {
        TargetNotDangerous = static_cast<int>(USV::DangerType::NotDangerous),
        TargetPotentiallyDangerous = static_cast<int>(USV::DangerType::PotentiallyDangerous),
        TargetDangerous = static_cast<int>(USV::DangerType::Dangerous),
        TargetUndefined,
        ShipOnWastedManeuver,
        TargetInitPosition,
        ShipOnRoute,
        ShipOnManeuver,
        ShipInitPosition,
        End
    };

    const USV::Ship* ship{};
    USV::Vector2 position{};
    double course{}; // radians
    double radius{};
    Type type{};
};

class GLVessels {
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> m_vessel_vbo{};
    std::unique_ptr<Buffer> m_vessels{};
    std::unique_ptr<Buffer> m_circle_vbo{};
    int m_viewLoc;
    const USV::CaseData* case_data_{};
private:
    std::vector<Vessel> vessels{};
    ListenerRemoveFunc list_listener_remove;
    ListenerRemoveFunc appearance_listener_remove;
public:
    GLVessels();
    ~GLVessels();

    [[nodiscard]] const USV::CaseData* getCaseData() const {
        return case_data_;
    }

    void setCaseData(const USV::CaseData* caseData) {
        case_data_ = caseData;
    }

    void render(glm::vec3 eyePos);

    void updatePositions(const std::vector<Vessel>& new_vessels);

    void updatePositions();

};


#endif //USV_GUI_GLVESSELS_H
