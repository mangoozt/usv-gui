#include "UsvRun.h"

#include <utility>

int USV::USVRunner::run(const std::filesystem::path& directory, const USV::InputTypes::DataFilenames* data_filenames) {
    std::string command{
            executable.string() + " " + "--target-settings" + " " +
            (directory / data_filenames->target_settings).string() + " " +
            "--targets" + " " + (directory / data_filenames->navigationProblem).string() + " " +
            "--settings" + " " + (directory / data_filenames->settings).string() + " " +
            "--nav-data" + " " + (directory / data_filenames->navigationParameters).string() + " " +
            "--hydrometeo" + " " + (directory / data_filenames->hydrometeo).string() + " " +
            "--constraints" + " " + (directory / data_filenames->constraints).string() + " " +
            "--route" + " " + (directory / data_filenames->route).string() + " " +
            "--maneuver" + " " + (directory / data_filenames->maneuvers).string() + " " +
            "--analyse" + " " + (directory / data_filenames->analyse).string() + " " +
            "--predict" + " " + (directory / data_filenames->targets_paths).string()};
    auto code = std::system(command.c_str());
    return code;
}

USV::USVRunner::USVRunner(std::filesystem::path  executable) : executable(std::move(executable)) {}
