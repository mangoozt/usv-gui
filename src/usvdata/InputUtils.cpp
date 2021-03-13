#include "InputUtils.h"
#include "InputDataJsonDefines.h"
#include <filesystem>

namespace USV::InputUtils {
    namespace {
        bool file_exists(const std::filesystem::path& filename) {
            return std::filesystem::exists(filename);
        }

        struct DataFilenames {
            std::string_view navigationParameters;
            std::string_view navigationProblem;
            std::string_view route;
            std::string_view maneuvers;
            std::string_view targets_paths;
            std::string_view targets_real_paths;
            std::string_view settings;
            std::string_view constraints;
        };

        constexpr DataFilenames data_filenames_native = {
                "nav-data.json",
                "target-data.json",
                "route-data.json",
                "maneuver.json",
                "target-maneuvers.json",
                "real-target-maneuvers.json",
                "settings.json",
                "constraints.json",

        };
        constexpr DataFilenames data_filenames_kt = {
                "navigation.json",
                "targets.json",
                "route.json",
                "result-maneuver.json",
                "predicted_tracks.json",
                "real-target-maneuvers.json",
                "settings.json",
                "constraints.json",
        };
    }

    InputTypes::InputData loadInputData(const std::string& data_directory) {
        InputTypes::InputData inputData;

        std::filesystem::path directory{data_directory};

        auto& filenames = (file_exists(directory / data_filenames_native.navigationParameters))
                          ? data_filenames_native : data_filenames_kt;

        load_from_json_file<true>(inputData.navigationParameters, directory / filenames.navigationParameters);
        load_from_json_file<true>(inputData.navigationProblem, directory / filenames.navigationProblem);
        load_from_json_file<true>(inputData.route, directory / filenames.route);
        load_from_json_file(inputData.maneuvers, directory / filenames.maneuvers);
        load_from_json_file(inputData.targets_paths, directory / filenames.targets_paths);
        load_from_json_file(inputData.targets_real_paths, directory / filenames.targets_real_paths);
        load_from_json_file<true>(inputData.settings, directory / filenames.settings);
        load_from_json_file(inputData.constraints, directory / filenames.constraints);
        return inputData;
    }

}
