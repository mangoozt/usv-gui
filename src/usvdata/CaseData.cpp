#include "CaseData.h"
#include "Frame.h"

namespace USV{
CaseData::CaseData(const InputTypes::InputData& input_data): radius(input_data.settings->manuever_calculation.safe_diverg_dist*0.5), route(0){
    Frame frame(input_data.navigationParameters->lat,input_data.navigationParameters->lon);
    route = Path(*input_data.route,frame);
    if(input_data.targets_paths)
        for(const auto&path:*input_data.targets_paths)
            targets_maneuvers.emplace_back(path,frame);

    if(input_data.targets_real_paths)
        for(const auto&path:*input_data.targets_real_paths)
            targets_real_maneuvers.emplace_back(path,frame);

    if(input_data.maneuvers)
        for(const auto&path:*input_data.maneuvers)
            maneuvers.emplace_back(path.path,frame);

}
}
