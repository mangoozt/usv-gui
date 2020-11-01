#include "CaseData.h"
#include "Frame.h"

namespace USV{
CaseData::CaseData(const InputTypes::InputData& input_data): radius(input_data.settings->manuever_calculation.safe_diverg_dist*0.5), route(0){
    Frame frame(input_data.navigationParameters->lat,input_data.navigationParameters->lon);
    route = Path(*input_data.route,frame);
    for(const auto&path:*input_data.targets_paths)
        targets_maneuvers.emplace_back(path,frame);

    for(const auto&path:*input_data.maneuvers)
        maneuvers.emplace_back(path.path,frame);

};
}
