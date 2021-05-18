#include "CaseData.h"

namespace USV {
    CaseData::CaseData(const InputTypes::InputData& input_data) :
            radius(input_data.settings->manuever_calculation.safe_diverg_dist * 0.5), analyse_result(
            input_data.analyse_result), frame(input_data.navigationParameters->lat,
                                              input_data.navigationParameters->lon), directory(input_data.directory)
            , data_filenames(input_data.data_filenames), start_time(input_data.navigationParameters->timestamp) {

        // LOAD OWN SHIP
        auto& nav_params = *input_data.navigationParameters;
        ownShip = {{nav_params.cat,
                           nav_params.timestamp,
                           {frame.fromWgs(nav_params.lat, nav_params.lon), nav_params.SOG, nav_params.COG},
                           "Own"}};
        if (input_data.maneuvers)
            for (const auto& path:*input_data.maneuvers)
                paths.emplace_back(PathType::ShipManeuver, &ownShip, Path(path.path, frame));

        paths.emplace_back(PathType::Route, &ownShip, Path(*input_data.route, frame));

        // LOAD TARGETS
        std::map<std::string, const InputTypes::AnalyseResult::TargetStatus*> target_statuses;
        if (analyse_result != nullptr) {
            for (const auto& ts: analyse_result->target_statuses)
                target_statuses.emplace(ts.id, &ts);
        }

        // check for paths
        std::vector<CurvedPath>* targets_paths = nullptr;
        std::vector<CurvedPath>* targets_real_paths = nullptr;
        if (input_data.targets_paths && input_data.targets_paths->size() == input_data.navigationProblem->size()) {
            targets_paths = input_data.targets_paths.get();
        }
        if (input_data.targets_real_paths &&
            input_data.targets_real_paths->size() == input_data.navigationProblem->size()) {
            targets_real_paths = input_data.targets_real_paths.get();
        }
        const auto& nav_problem = *input_data.navigationProblem;
        targets.reserve(nav_problem.size());
        for (size_t i = 0; i < nav_problem.size(); ++i) {
            targets.push_back({
                                      {nav_problem[i].cat,
                                              nav_problem[i].timestamp,
                                              {frame.fromWgs(nav_problem[i].lat, nav_problem[i].lon),
                                                      nav_problem[i].SOG,
                                                      nav_problem[i].COG},
                                              nav_problem[i].id,
                                              (analyse_result != nullptr ? target_statuses.find(
                                                      nav_problem[i].id)->second : nullptr)
                                      }});

            if (targets_paths)
                paths.emplace_back(PathType::TargetManeuver, &targets[i], Path(input_data.targets_paths->at(i), frame));
            if (targets_real_paths)
                paths.emplace_back(PathType::TargetRealManeuver, &targets[i], Path(input_data.targets_real_paths->at(i), frame));
        }

        for (const auto& pe: paths) {
            min_time = std::min(pe.path.getStartTime(), min_time);
            max_time = std::max(pe.path.endTime(), max_time);
        }

        // END LOAD TARGETS

        if (input_data.constraints) {
            restrictions = Restrictions::Restrictions(*input_data.constraints, frame);
        }
    }
}
