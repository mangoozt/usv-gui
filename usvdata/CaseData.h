#ifndef CASEDATA_H
#define CASEDATA_H

#include "InputTypes.h"
#include "Path.h"

#include <vector>

namespace USV{
struct Vessel{
    Vector2 position;
    double course;
    double color[4]{0,0,0,1};
};


struct CaseData{
    std::vector<Vessel> vessels;
    Path route;

    CaseData(const InputTypes::InputData& input_data);
};


}

#endif // CASEDATA_H
