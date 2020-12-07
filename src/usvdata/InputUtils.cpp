#include "InputUtils.h"

namespace USV::InputUtils {

InputTypes::InputData loadInputData(const std::string & data_directory){
    InputTypes::InputData inputData;
#ifdef _WIN32
    char sep{'\\'};
#else
    char sep{'/'};
#endif
    load_from_json_file(inputData.navigationParameters, data_directory+sep+"nav-data.json");
    load_from_json_file(inputData.navigationProblem, data_directory+sep+"target-data.json");
    load_from_json_file(inputData.route, data_directory+sep+"route-data.json");
    load_from_json_file(inputData.maneuvers, data_directory+sep+"maneuver.json");
    load_from_json_file(inputData.targets_paths, data_directory+sep+"target-maneuvers.json");
    load_from_json_file(inputData.targets_real_paths, data_directory+sep+"real-target-maneuvers.json");
    load_from_json_file(inputData.settings, data_directory+sep+"settings.json");
    load_from_json_file(inputData.constraints, data_directory+sep+"constraints.json");
    return inputData;
}

}
