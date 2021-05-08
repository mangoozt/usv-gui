#include "CaseData.h"

namespace USV {
    CaseData::CaseData(const InputTypes::InputData& input_data) :
            radius(input_data.settings->manuever_calculation.safe_diverg_dist * 0.5), route(0),
            frame(input_data.navigationParameters->lat, input_data.navigationParameters->lon),
            directory(input_data.directory), data_filenames(input_data.data_filenames),
            start_time(input_data.navigationParameters->timestamp) {
        route = Path(*input_data.route, frame);
        vessel_names.reserve(input_data.targets_paths->size() + 1);
        vessel_names.emplace_back("Own");
        for (auto& target:*input_data.navigationProblem) {
            vessel_names.push_back(target.id);
        }
        if (input_data.targets_paths)
            for (const auto& path:*input_data.targets_paths)
                targets_maneuvers.emplace_back(path, frame);

        if (input_data.targets_real_paths)
            for (const auto& path:*input_data.targets_real_paths)
                targets_real_maneuvers.emplace_back(path, frame);

        if (input_data.maneuvers)
            for (const auto& path:*input_data.maneuvers)
                maneuvers.emplace_back(path.path, frame);

        if (input_data.constraints) {
            restrictions = Restrictions::Restrictions(*input_data.constraints, frame);
        }
    }
}
