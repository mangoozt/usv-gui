#include "InputUtils.h"
#include "InputDataJsonDefines.h"
#include <filesystem>

namespace USV::InputUtils {
    namespace {
        bool file_exists(const std::filesystem::path& filename) {
            return std::filesystem::exists(filename);
        }

        constexpr InputTypes::DataFilenames data_filenames_native = {
                "nav-data.json",
                "target-data.json",
                "route-data.json",
                "maneuver.json",
                "target-maneuvers.json",
                "real-target-maneuvers.json",
                "settings.json",
                "constraints.json",
                "hmi-data.json",
                "nav-report.json",
                "target-settings.json"
        };
        constexpr InputTypes::DataFilenames data_filenames_kt = {
                "navigation.json",
                "targets.json",
                "route.json",
                "result_maneuver.json",
                "predicted_tracks.json",
                "real-target-maneuvers.json",
                "settings.json",
                "constraints.json",
                "hydrometeo.json",
                "evaluation.json",
                "targets_settings.json"
        };
        constexpr InputTypes::DataFilenames data_filenames_argument_like = {
                "nav-data.json",
                "targets.json",
                "route.json",
                "maneuver.json",
                "predict.json",
                "real-target-maneuvers.json",
                "settings.json",
                "constraints.json",
                "hydrometeo.json",
                "analyse.json",
                "targets-settings.json"
        };
    }

    InputTypes::InputData loadInputData(const std::string& data_directory) {
        InputTypes::InputData data;

        data.directory = {data_directory};
        if(file_exists(data.directory / data_filenames_native.navigationParameters)){
            if(file_exists(data.directory / data_filenames_native.navigationProblem)){
                data.data_filenames = &data_filenames_native;
            }else{
                data.data_filenames = &data_filenames_argument_like;
            }
        }else{
            data.data_filenames = &data_filenames_kt;
        }


        auto& filenames = *data.data_filenames;

        load_from_json_file<true>(data.navigationParameters, data.directory / filenames.navigationParameters);
        load_from_json_file<true>(data.navigationProblem, data.directory / filenames.navigationProblem);
        load_from_json_file<true>(data.route, data.directory / filenames.route);
        load_from_json_file(data.maneuvers, data.directory / filenames.maneuvers);
        load_from_json_file(data.targets_paths, data.directory / filenames.targets_paths);
        load_from_json_file(data.targets_real_paths, data.directory / filenames.targets_real_paths);
        load_from_json_file<true>(data.settings, data.directory / filenames.settings);
        load_from_json_file(data.constraints, data.directory / filenames.constraints);
        load_from_json_file(data.analyse_result, data.directory / filenames.analyse);
        load_from_json_file(data.wasted_maneuvers, data.directory / "wasted_maneuvers.json");
        return data;
    }

}
