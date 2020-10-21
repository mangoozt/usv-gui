#include "CaseData.h"
#include "Frame.h"

namespace USV{
CaseData::CaseData(const InputTypes::InputData& input_data):route(0){
    Frame frame(input_data.navigationParameters->lat,input_data.navigationParameters->lon);

    vessels.push_back({{0,0},input_data.navigationParameters->COG/180*M_PI,{1,0,0,1}});
    for(const auto& target: *input_data.navigationProblem){
        vessels.push_back({frame.fromWgs(target.lat,target.lon),target.COG/180*M_PI,{0,1,0,1}});
    }
    route = Path(*input_data.route,frame);
};
}
