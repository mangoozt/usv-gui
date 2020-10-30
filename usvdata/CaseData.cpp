#include "CaseData.h"
#include "Frame.h"

namespace USV{
CaseData::CaseData(const InputTypes::InputData& input_data):route(0){
    Frame frame(input_data.navigationParameters->lat,input_data.navigationParameters->lon);
    auto radius = input_data.settings->manuever_calculation.safe_diverg_dist;
    vessels.push_back({{0,0},input_data.navigationParameters->COG/180*M_PI,{0,1,0},radius});
    for(const auto& target: *input_data.navigationProblem){
        vessels.push_back({frame.fromWgs(target.lat,target.lon),target.COG/180*M_PI,{0,0,1},radius});
    }
    route = Path(*input_data.route,frame);
    for(const auto&path:*input_data.targets_paths)
        targets_maneuvers.emplace_back(path,frame);

    for(const auto&path:*input_data.maneuvers)
        maneuvers.emplace_back(path.path,frame);

};
}
