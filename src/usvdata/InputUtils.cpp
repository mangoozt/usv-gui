#include "InputUtils.h"

namespace USV::InputUtils {
namespace fs=std::filesystem;

InputTypes::InputData loadInputData(const fs::path & data_directory){
    namespace fs = std::filesystem;
    InputTypes::InputData inputData;
    load_from_json_file(inputData.navigationParameters, fs::path(data_directory)/"nav-data.json");
    load_from_json_file(inputData.navigationProblem, fs::path(data_directory)/"target-data.json");
    load_from_json_file(inputData.route, fs::path(data_directory)/"route-data.json");
    load_from_json_file(inputData.maneuvers, fs::path(data_directory)/"maneuver.json");
    load_from_json_file(inputData.targets_paths, fs::path(data_directory)/"target-maneuvers.json");
    load_from_json_file(inputData.targets_real_paths, fs::path(data_directory)/"real-target-maneuvers.json");
    load_from_json_file(inputData.settings, fs::path(data_directory)/"settings.json");
    return inputData;
}

}
