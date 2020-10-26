#ifndef CASEDATA_H
#define CASEDATA_H

#include "InputTypes.h"
#include "Path.h"

#include <vector>

namespace USV{
struct Vessel{
    Vector2 position;
    double course; // radians
    double color[3]{0,0,0};
};


struct CaseData{
    std::vector<Vessel> vessels{0};
    Path route{0};
    std::vector<Path> targets_maneuvers;
    std::vector<Path> maneuvers;

    CaseData() = default;
    CaseData(const InputTypes::InputData& input_data);
};


}

#endif // CASEDATA_H
