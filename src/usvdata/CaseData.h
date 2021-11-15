#ifndef CASEDATA_H
#define CASEDATA_H

#include "InputTypes.h"
#include "Path.h"
#include "Restrictions.h"

#include <utility>
#include <vector>
#include <map>

namespace USV {

    enum class PathType {
        TargetManeuver = 0,
        WastedManeuver,
        ShipManeuver,
        Route,
        End
    };

    struct Ship {
        using TargetStatus = InputTypes::AnalyseResult::TargetStatus;
        ShipCategory category;
        time_t initTimestamp;
        Path::Position initPosition;
        std::string name;
        const TargetStatus* target_status{};
    };

    struct OwnShip : public Ship {
    };

    struct Target : public Ship {
    };

    struct PathEnvelope {
        PathType pathType;
        const Ship* ship;
        Path path;

        inline PathEnvelope(PathType path_type, const Ship* ship, Path& path) :
                pathType(path_type), ship(ship), path(path) {}

        inline PathEnvelope(PathType path_type, const Ship* ship, Path path) :
                pathType(path_type), ship(ship), path(std::move(path)) {}

    };

    struct CaseData {
        double radius{};
        OwnShip ownShip;
        std::vector<Target> targets{0};
        std::shared_ptr<InputTypes::AnalyseResult> analyse_result;
        Restrictions::Restrictions restrictions;
        Frame frame;
        double min_time{std::numeric_limits<double>::infinity()};
        double max_time{0};
        std::filesystem::path directory;
        const InputTypes::DataFilenames* data_filenames;
        time_t start_time;

        std::vector<PathEnvelope> paths;

        explicit CaseData(const InputTypes::InputData& input_data);

//        CaseData(const CaseData& o) : frame(o.frame),
//                max_time(o.max_time),
//                min_time(o.min_time),
//                data_filenames(o.data_filenames),
//                start_time(o.start_time) {
//        }
//        CaseData(const CaseData&& o) noexcept: frame(o.frame), max_time(o.max_time), min_time(o.min_time)
//                , data_filenames(o.data_filenames), start_time(o.start_time), directory(o.directory) {
//        }
//        std::vector<USV::Vessel> get_positions(double time) {
//            std::vector<USV::Vessel> vessels;
//            USV::Color color{0.8f, 0.8f, 0.8f};
//            push_position(time, case_data->route, vessels, color, case_data->radius);
//
//            color = {0, 0, 1};
//            for (size_t i = 0; i < case_data->targets_maneuvers.size(); ++i)
//                push_position(time, case_data->targets_maneuvers[i], vessels, color, case_data->radius);
//
//            color = {0, 1, 0};
//            for (const auto& maneuver: case_data->maneuvers)
//                push_position(time, maneuver, vessels, color, case_data->radius);
//
//            map.updatePositions(vessels);
//            map.updateTime(time / 3600);
//            map.updateSunAngle(static_cast<long>(time), case_data->frame.getRefLat(), case_data->frame.getRefLon());
//
//            if (time_label) {
//                time_t seconds = static_cast<time_t>(time) - case_data->start_time;
//                auto hours = seconds / 3600;
//                seconds = seconds % 3600;
//                auto minutes = seconds / 60;
//                seconds = seconds % 60;
//                char t_plus_str[32];
//                snprintf(t_plus_str, sizeof(t_plus_str), "(T+%02ld:%02ld:%02d)", hours, minutes, (int) seconds);
//                time_label->set_value(std::to_string((int) (time)) + t_plus_str);
//                time_label->focus_event(true);
//                time_label->focus_event(false);
//            }
//        }
    };
}

#endif // CASEDATA_H
