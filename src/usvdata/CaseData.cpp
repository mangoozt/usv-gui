#include "CaseData.h"

namespace USV {
    CaseData::CaseData(const InputTypes::InputData& input_data) :
            radius(input_data.settings->manuever_calculation.safe_diverg_dist * 0.5), analyse_result(
            input_data.analyse_result), frame(input_data.navigationParameters->lat,
                                              input_data.navigationParameters->lon), directory(input_data.directory)
            , data_filenames(input_data.data_filenames), start_time(input_data.navigationParameters->timestamp) {

        // LOAD OWN SHIP

        //
        //{localPos.y(),localPos.x()}, M_PI_2 - degrees_to_radians(segment.begin_angle)
        auto& nav_params = *input_data.navigationParameters;
        auto localPos = frame.fromWgs(nav_params.lat, nav_params.lon);
        ownShip = {{nav_params.cat,
                           nav_params.timestamp,
                           {{localPos.y(), localPos.x()}, M_PI_2 - degrees_to_radians(nav_params.COG), nav_params.SOG},
                           "Own"}};
        if (input_data.maneuvers)
            for (const auto& path:*input_data.maneuvers)
                paths.emplace_back(&ownShip, Path(path.path, frame, PathType::ShipManeuver));

        paths.emplace_back(&ownShip, Path(*input_data.route, frame, PathType::Route));

        // LOAD TARGETS
        std::map<std::string, const InputTypes::AnalyseResult::TargetStatus*> target_statuses;
        if (analyse_result != nullptr) {
            for (const auto& ts: analyse_result->target_statuses)
                target_statuses.emplace(ts.id, &ts);
        }

        // check for paths
        std::vector<CurvedPath>* targets_paths = nullptr;
        if (input_data.targets_paths && input_data.targets_paths->size() == input_data.navigationProblem->size()) {
            targets_paths = input_data.targets_paths.get();
        }
        if (input_data.targets_real_paths &&
            input_data.targets_real_paths->size() == input_data.navigationProblem->size()) {
        }

        if(input_data.wasted_maneuvers){
            for(const auto& solver_wasted : *input_data.wasted_maneuvers){
                for(const auto& m : solver_wasted){
                    paths.emplace_back(&ownShip, Path(m, frame, PathType::WastedManeuver));
                }
            }
        }

        const auto& nav_problem = *input_data.navigationProblem;
        targets.reserve(nav_problem.size());
        for (size_t i = 0; i < nav_problem.size(); ++i) {
            localPos = frame.fromWgs(nav_problem[i].lat, nav_problem[i].lon);
            const InputTypes::AnalyseResult::TargetStatus* target_status{};
            if(analyse_result != nullptr) {
                auto traget_status_itr = target_statuses.find(nav_problem[i].id);
                target_status = (traget_status_itr != target_statuses.end() ? traget_status_itr->second : nullptr);
            }


            targets.push_back({
                                      {nav_problem[i].cat,
                                              nav_problem[i].timestamp,
                                              {{localPos.y(), localPos.x()},
                                                      M_PI_2 - degrees_to_radians(nav_problem[i].COG),
                                                      nav_problem[i].SOG
                                                      },
                                              nav_problem[i].id,
                                              target_status
                                      }});

            if (targets_paths)
                paths.emplace_back(&targets[i], Path(input_data.targets_paths->at(i), frame, PathType::TargetManeuver));
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
