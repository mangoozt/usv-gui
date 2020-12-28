#ifndef CASEDATA_H
#define CASEDATA_H

#include "InputTypes.h"
#include "Path.h"
#include "Restrictions.h"

#include <vector>

namespace USV {

    struct Color {
        float r;
        float g;
        float b;
        float a{1};
    };

    struct Vessel {
        Vector2 position;
        double course; // radians
        double radius;
        Color color{0, 0, 0};
    };

    struct CaseData {
        double radius;
        std::vector<Vessel> vessels{0};
        std::vector<std::string> vessel_names{};
        Path route{0};
        std::vector<Path> targets_maneuvers;
        std::vector<Path> targets_real_maneuvers;
        std::vector<Path> maneuvers;
        Restrictions::Restrictions restrictions;

        CaseData() = default;

        explicit CaseData(const InputTypes::InputData& input_data);
    };
}

#endif // CASEDATA_H
