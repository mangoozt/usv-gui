#ifndef USV_GUI_INPUTDATAJSONDEFINES_H
#define USV_GUI_INPUTDATAJSONDEFINES_H

#include "CurvedPath.h"
#include "InputTypes.h"
#include <spotify/json.hpp>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace spotify::json {
    using namespace USV;
    using namespace USV::InputTypes;
    using namespace codec;
// Specialize spotify::json::default_codec_t to specify default behavior when
// encoding and decoding objects of certain types.

    template<>
    struct default_codec_t<ShipCategory> {
        static int CatDecode(const ShipCategory cat) {
            return static_cast<int>(cat);
        }

        static ShipCategory CatEncode(const int cat) {
            return static_cast<ShipCategory>(cat);
        }

        static auto codec() {
            return codec::transform(codec::number<int>(), CatDecode, CatEncode);
        }
    };

    template<>
    struct default_codec_t<NavigationParameters> {
        static codec::object_t<NavigationParameters> codec() {
            auto codec = codec::object<NavigationParameters>();

//        const codec::any_codec_t<ShipCategory> cat_codec = any_codec(codec::);
            codec.required("cat", &NavigationParameters::cat);
            codec.required("lat", &NavigationParameters::lat);
            codec.required("lon", &NavigationParameters::lon);
            codec.required("SOG", &NavigationParameters::SOG);
            codec.required("COG", &NavigationParameters::COG);
            codec.required("STW", &NavigationParameters::STW);
            codec.required("heading", &NavigationParameters::heading);
            codec.required("width", &NavigationParameters::width);
            codec.required("length", &NavigationParameters::length);
            codec.required("width_offset", &NavigationParameters::width_offset);
            codec.required("length_offset", &NavigationParameters::length_offset);
            codec.required("timestamp", &NavigationParameters::timestamp);
            return codec;
        }
    };

    template<>
    struct default_codec_t<TargetParameters> {
        static codec::object_t<TargetParameters> codec() {
            auto codec = codec::object<TargetParameters>();
            codec.required("id", &TargetParameters::id);
            codec.required("cat", &TargetParameters::cat);
            codec.required("lat", &TargetParameters::lat);
            codec.required("lon", &TargetParameters::lon);
            codec.required("SOG", &TargetParameters::SOG);
            codec.required("COG", &TargetParameters::COG);
            codec.optional("width", &TargetParameters::width);
            codec.optional("length", &TargetParameters::length);
            codec.optional("width_offset", &TargetParameters::width_offset);
            codec.optional("length_offset", &TargetParameters::length_offset);
            codec.required("first_detect_dist", &TargetParameters::first_detect_dist);
            codec.optional("cross_dist", &TargetParameters::cross_dist);
            codec.required("timestamp", &TargetParameters::timestamp);
            return codec;
        }
    };

    template<>
    struct default_codec_t<Maneuver> {
        static int SolutionTypeDecode(const Maneuver::SolutionType cat) {
            return static_cast<int>(cat);
        }

        static Maneuver::SolutionType SolutionTypeEncode(const int cat) {
            return static_cast<Maneuver::SolutionType>(cat);
        }

        static codec::object_t<Maneuver> codec() {
            auto codec = codec::object<Maneuver>();
            auto type_codec = codec::transform(codec::number<int>(), SolutionTypeDecode, SolutionTypeEncode);
            codec.required("solution_type", &Maneuver::solution_type, type_codec);
            codec.required("path", &Maneuver::path);
            codec.required("msg", &Maneuver::msg);
            codec.required("solver_name", &Maneuver::solver_name);
            return codec;
        }
    };

    template<>
    struct default_codec_t<Settings> {
        static int ManeuverWayDecode(const Settings::ManeuverCalculation::ManeuverWay m_type) {
            return static_cast<int>(m_type);
        }

        static Settings::ManeuverCalculation::ManeuverWay ManeuverWayEncode(const int m_type) {
            return static_cast<Settings::ManeuverCalculation::ManeuverWay>(m_type);
        }

        static codec::object_t<Settings> codec() {

            using ManeuverCalculation = Settings::ManeuverCalculation;
            auto maneuver_calc = codec::object<ManeuverCalculation>();
            maneuver_calc.required("priority", &ManeuverCalculation::priority);
            auto type_codec = codec::transform(codec::number<int>(), ManeuverWayDecode, ManeuverWayEncode);
            maneuver_calc.required("maneuver_way", &ManeuverCalculation::maneuver_way, type_codec);
            maneuver_calc.required("safe_diverg_dist", &ManeuverCalculation::safe_diverg_dist);
            maneuver_calc.required("min_diverg_dist", &ManeuverCalculation::min_diverg_dist);
            maneuver_calc.required("minimal_speed", &ManeuverCalculation::minimal_speed);
            maneuver_calc.required("maximal_speed", &ManeuverCalculation::maximal_speed);
            maneuver_calc.required("forward_speed1", &ManeuverCalculation::forward_speed1);
            maneuver_calc.required("forward_speed2", &ManeuverCalculation::forward_speed2);
            maneuver_calc.required("forward_speed3", &ManeuverCalculation::forward_speed3);
            maneuver_calc.required("forward_speed4", &ManeuverCalculation::forward_speed4);
            maneuver_calc.required("forward_speed5", &ManeuverCalculation::forward_speed5);
            maneuver_calc.required("reverse_speed1", &ManeuverCalculation::reverse_speed1);
            maneuver_calc.required("reverse_speed2", &ManeuverCalculation::reverse_speed2);
            maneuver_calc.required("max_course_delta", &ManeuverCalculation::max_course_delta);
            maneuver_calc.required("time_advance", &ManeuverCalculation::time_advance);
            maneuver_calc.required("can_leave_route", &ManeuverCalculation::can_leave_route);
            maneuver_calc.required("max_route_deviation", &ManeuverCalculation::max_route_deviation);
            maneuver_calc.required("max_circulation_radius", &ManeuverCalculation::max_circulation_radius);
            maneuver_calc.required("min_circulation_radius", &ManeuverCalculation::min_circulation_radius);
            maneuver_calc.required("breaking_distance", &ManeuverCalculation::breaking_distance);
            maneuver_calc.required("run_out_distance", &ManeuverCalculation::run_out_distance);
            maneuver_calc.required("forecast_time", &ManeuverCalculation::forecast_time);

            using SafetyControl = Settings::SafetyControl;
            auto safety_control = codec::object<SafetyControl>();
            safety_control.required("cpa", &SafetyControl::cpa);
            safety_control.required("tcpa", &SafetyControl::tcpa);
            safety_control.required("min_detect_dist", &SafetyControl::min_detect_dist);
            safety_control.required("last_moment_dist", &SafetyControl::last_moment_dist);

            auto safety_zone = codec::object<SafetyControl::SafetyZone>();
            safety_zone.required("safety_zone_type", &SafetyControl::SafetyZone::safety_zone_type);
            safety_zone.optional("radius", &SafetyControl::SafetyZone::radius);
            safety_zone.optional("start_angle", &SafetyControl::SafetyZone::start_angle);
            safety_zone.optional("end_angle", &SafetyControl::SafetyZone::end_angle);
            safety_zone.optional("length", &SafetyControl::SafetyZone::length);
            safety_zone.optional("width", &SafetyControl::SafetyZone::width);
            safety_zone.optional("lengthOffset", &SafetyControl::SafetyZone::lengthOffset);

            safety_control.required("safety_zone", &SafetyControl::safety_zone, safety_zone);

            auto codec = codec::object<Settings>();
            codec.required("maneuver_calculation", &Settings::manuever_calculation, maneuver_calc);
            codec.required("safety_control", &Settings::safety_control, safety_control);
            return codec;
        }
    };


    template<>
    struct default_codec_t<Vector2> {
        static auto codec() {
            return codec::transform(
                    codec::pair(codec::number<double>(), codec::number<double>()),
                    [](const Vector2& p) { return std::make_pair(p.x(), p.y()); },
                    [](const std::pair<double, double>& p) { return Vector2(p.first, p.second); });
        }
    };

    template<>
    struct default_codec_t<FeatureGeometry> {
        static auto codec() {
            codec::object_t<FeatureGeometry> codec_point;
            codec_point.required("type", codec::eq(std::string("Point")));
            codec_point.required("type", &FeatureGeometry::type,
                                 spotify::json::codec::ignore(GeometryType::GeometryPoint));
            codec_point.required("coordinates", &FeatureGeometry::coordinatesPoint);

            codec::object_t<FeatureGeometry> codec_polygon;
            codec_polygon.required("type", codec::eq(std::string("Polygon")));
            codec_polygon.required("type", &FeatureGeometry::type,
                                   spotify::json::codec::ignore(GeometryType::GeometryPolygon));
            codec_polygon.required("coordinates", &FeatureGeometry::coordinatesPolygon);

            codec::object_t<FeatureGeometry> codec_linestring;
            codec_linestring.required("type", codec::eq(std::string("LineString")));
            codec_linestring.required("type", &FeatureGeometry::type,
                                      spotify::json::codec::ignore(GeometryType::GeometryLine));
            codec_linestring.required("coordinates", &FeatureGeometry::coordinatesLine);

            return codec::one_of(codec_point, codec_linestring, codec_polygon);
        }
    };

    template<>
    struct default_codec_t<Feature> {
        static codec::object_t<Feature> codec() {
            auto codec = codec::object<Feature>();
            codec.required("properties", &Feature::properties);
            codec.required("geometry", &Feature::geometry);
            return codec;
        }
    };

    template<>
    struct default_codec_t<FeatureCollection> {
        static codec::object_t<FeatureCollection> codec() {
            auto codec = codec::object<FeatureCollection>();
            codec.required("features", &FeatureCollection::features);
            return codec;
        }
    };

    template<>
    struct default_codec_t<USV::FeatureProperties> {
        using FP = FeatureProperties;
        using LT = LimitationType;
        using RT = RestrictionType;

        static codec::object_t<FP> codec() {
            const static auto limitation_type_codec = codec::enumeration<LT, std::string>(
                    {
                            {LT::point_approach_prohibition,     "point_approach_prohibition"},
                            {LT::line_crossing_prohibition,      "line_crossing_prohibition"},
                            {LT::zone_entering_prohibition,      "zone_entering_prohibition"},
                            {LT::zone_leaving_prohibition,       "zone_leaving_prohibition"},
                            {LT::movement_parameters_limitation, "movement_parameters_limitation"}
                    });

            const static auto hardness_type_codec = codec::enumeration<RT, std::string>(
                    {
                            {RT::Hard, "hard"},
                            {RT::Soft, "soft"}
                    });

            auto codec = codec::object<FP>();
            codec.required("id", &FP::id);
            codec.required("limitation_type", &FP::limitation_type, limitation_type_codec);
            codec.required("hardness", &FP::hardness, hardness_type_codec);
            codec.required("source_id", &FP::source_id);
            codec.required("source_object_code", &FP::source_object_code);

            codec.optional("max_course", &FP::max_course);
            codec.optional("min_course", &FP::min_course);
            codec.optional("max_speed", &FP::max_speed);
            return codec;
        }
    };


// Specialize spotify::json::default_codec_t to specify default behavior when
// encoding and decoding objects of certain types.
    template<>
    struct default_codec_t<CurvedPath> {
        static codec::object_t<CurvedPath> codec() {
            auto codec = codec::object<CurvedPath>();
            codec.required("items", &CurvedPath::items);
            codec.required("start_time", &CurvedPath::start_time);
            return codec;
        }
    };

    template<>
    struct default_codec_t<CurvedPath::Segment> {
        static codec::object_t<CurvedPath::Segment> codec() {
            auto codec = codec::object<CurvedPath::Segment>();
            codec.required("lat", &CurvedPath::Segment::lat);
            codec.required("lon", &CurvedPath::Segment::lon);
            codec.required("begin_angle", &CurvedPath::Segment::begin_angle);
            codec.required("curve", &CurvedPath::Segment::curve);
            codec.required("length", &CurvedPath::Segment::length);
            codec.required("duration", &CurvedPath::Segment::duration);
            codec.optional("port_dev", &CurvedPath::Segment::port_dev);
            codec.optional("starboard_dev", &CurvedPath::Segment::starboard_dev);
            return codec;
        }
    };
}

namespace USV::InputUtils {
    template<bool R = false, typename T>
    void load_from_json_file(std::unique_ptr<T>& data, const std::filesystem::path& filename) {
        using namespace spotify::json;

        if (filename.empty()) {
            if constexpr (R) { throw std::runtime_error("Empty filename "); }
            else { return; }
        }
        std::cout << "Loading `" << filename << "` ... ";
        std::ifstream ifs(filename);
        if (!ifs.good()) {
            std::cout << "failed to open" << std::endl;
            if constexpr (R) { throw std::runtime_error("Failed to open " + filename.string()); }
            else { return; }
        }
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        data = std::make_unique<T>();
        if (!try_decode<T>(*data, buffer.str())) {
            std::cout << "failed to parse" << std::endl;
            throw std::runtime_error("Failed to parse " + filename.string());
        }
    }

}

#endif //USV_GUI_INPUTDATAJSONDEFINES_H
