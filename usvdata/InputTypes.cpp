#include "InputTypes.h"
#include "InputUtils.h"
#include <iostream>

namespace USV {
    namespace {
        typedef std::string strType;

        using namespace InputUtils;

        const struct {
            const strType cat{"cat"};
            const strType lat{"lat"};
            const strType lon{"lon"};
            const strType SOG{"SOG"};
            const strType COG{"COG"};
            const strType STW{"STW"};
            const strType heading{"heading"};
            const strType width{"width"};
            const strType length{"length"};
            const strType width_offset{"width_offset"};
            const strType length_offset{"length_offset"};
            const strType timestamp{"timestamp"};
        } navigationParametersFieldNames;

        const struct {
            const strType id = "id";
            const strType cat = "cat";
            const strType lat = "lat";
            const strType lon = "lon";
            const strType SOG = "SOG";
            const strType COG = "COG";
            const strType width = "width";
            const strType length = "length";
            const strType width_offset = "width_offset";
            const strType length_offset = "length_offset";
            const strType first_detect_dist = "first_detect_dist";
            const strType cross_dist = "cross_dist";
            const strType timestamp = "timestamp";
        } vehicleParametersFieldNames;

        const struct {
            const strType id = "id";
            const strType limitation_type = "limitation_type";
            const strType hardness = "hardness";
            const strType source_id = "source_id";
            const strType source_object_code = "source_object_code";
            const strType distance = "distance";
            const strType max_course = "max_course";
            const strType min_course = "min_course";
            const strType max_speed = "max_speed";
        } featurePropertiesFieldNames;
    }

    GeometryType GeometryTypeFromString(const std::string& value) {
        if (value == "Point") return GeometryPoint;
        if (value == "LineString") return GeometryLine;
        if (value == "Polygon") return GeometryPolygon;
        throw std::runtime_error("Wrong Geometry type");
    }

    std::ostream& operator<<(std::ostream& os, const USV::Polygon& poly) {
        os << sParenthesisL;
        for (size_t i = 0; i < poly.size() - 1; ++i) {
            os << sParenthesisL << poly[i] << sParenthesisRComma;
        }
        os << sParenthesisL << poly.back() << "))";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Geometry& geom) {
        using namespace std;
        switch (geom.type) {
            case GeometryPoint:
                os << "Point(" << geom.coordinatesPoint << sParenthesisR << endl;
                break;
            case GeometryLine:
                os << "Line(";
                for (size_t i = 0; i < geom.coordinatesLine.size() - 1; ++i) {
                    os << sParenthesisL << geom.coordinatesLine[i] << sParenthesisRComma;
                }
                os << sParenthesisL << geom.coordinatesLine.back() << sParenthesisR;
                os << sParenthesisR << endl;
                break;
            case GeometryPolygon:
                os << "Poly(";
                for (size_t i = 0; i < geom.coordinatesPolygon.size() - 1; ++i) {
                    os << sParenthesisL << geom.coordinatesPolygon[i] << sParenthesisRComma << endl;
                }
                os << sParenthesisL << geom.coordinatesPolygon.back() << sParenthesisR;
                os << sParenthesisR << endl;
                break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const USV::NavigationParameters& np) {
        auto& fieldnames = navigationParametersFieldNames;

        os << "\t" << fieldnames.cat << sColon << np.cat << std::endl;
        os << "\t" << fieldnames.lon << sColon << np.lon << std::endl;
        os << "\t" << fieldnames.lat << sColon << np.lat << std::endl;
        os << "\t" << fieldnames.COG << sColon << np.COG << std::endl;
        os << "\t" << fieldnames.SOG << sColon << np.SOG << std::endl;
        os << "\t" << fieldnames.STW << sColon << np.STW << std::endl;
        os << "\t" << fieldnames.heading << sColon << np.heading << std::endl;
        os << "\t" << fieldnames.width << sColon << np.width << std::endl;
        os << "\t" << fieldnames.length << sColon << np.length << std::endl;
        os << "\t" << fieldnames.width_offset << sColon << np.width_offset << std::endl;
        os << "\t" << fieldnames.length_offset << sColon << np.length_offset << std::endl;
        os << "\t" << fieldnames.timestamp << sColon << np.timestamp << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const USV::VehicleParameters& vp) {
        auto& fieldnames = vehicleParametersFieldNames;

        os << "\t" << fieldnames.id << sColon << vp.id << std::endl;
        os << "\t" << fieldnames.cat << sColon << vp.cat << std::endl;
        os << "\t" << fieldnames.lon << sColon << vp.lon << std::endl;
        os << "\t" << fieldnames.lat << sColon << vp.lat << std::endl;
        os << "\t" << fieldnames.COG << sColon << vp.COG << std::endl;
        os << "\t" << fieldnames.SOG << sColon << vp.SOG << std::endl;
        os << "\t" << fieldnames.width << sColon << vp.width << std::endl;
        os << "\t" << fieldnames.length << sColon << vp.length << std::endl;
        os << "\t" << fieldnames.width_offset << sColon << vp.width_offset << std::endl;
        os << "\t" << fieldnames.length_offset << sColon << vp.length_offset << std::endl;
        os << "\t" << fieldnames.first_detect_dist << sColon << vp.first_detect_dist << std::endl;
        os << "\t" << fieldnames.cross_dist << sColon << vp.cross_dist << std::endl;
        os << "\t" << fieldnames.timestamp << sColon << vp.timestamp << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const USV::NavigationProblem& np) {
        size_t i = 0;
        for (auto& vehicle:np.vehicles) {
            os << "Vehicle #" << i++ << std::endl;
            os << vehicle << std::endl;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const USV::Feature& feature) {
        auto& fieldnames = featurePropertiesFieldNames;

        os << feature.geometry << std::endl;
        os << "properties:" << std::endl;
        os << "\t" << fieldnames.id << sColon << feature.properties.id << std::endl;
        os << "\t" << fieldnames.limitation_type << sColon << feature.properties.limitation_type
           << std::endl;
        os << "\t" << fieldnames.hardness << sColon << feature.properties.hardness << std::endl;
        os << "\t" << fieldnames.source_id << sColon << feature.properties.source_id << std::endl;
        os << "\t" << fieldnames.distance << sColon << feature.properties.distance << std::endl;
        os << "\t" << fieldnames.max_course << sColon << feature.properties.max_course << std::endl;
        os << "\t" << fieldnames.min_course << sColon << feature.properties.min_course << std::endl;
        os << "\t" << fieldnames.max_speed << sColon << feature.properties.max_speed << std::endl;

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const USV::FeatureCollection& fc) {
        size_t i = 0;
        for (auto& feature:fc.features) {
            os << "Feature #" << i++ << std::endl;
            os << feature << std::endl;
        }
        return os;
    }

    bool operator==(const USV::VehicleParameters& a, const USV::VehicleParameters& b) {
        return a.id == b.id &&
               a.cat == b.cat &&
               a.lat == b.lat &&
               a.lon == b.lon &&
               a.SOG == b.SOG &&
               a.COG == b.COG &&
               a.width == b.width &&
               a.length == b.length &&
               a.width_offset == b.width_offset &&
               a.length_offset == b.length_offset &&
               a.first_detect_dist == b.first_detect_dist &&
               a.cross_dist == b.cross_dist &&
               a.timestamp == b.timestamp;

    }

    namespace {
        //Helping function overloading for ShipCategory
        void fillFromJson(const char* fieldName, ShipCategory& field, const rapidjson::Value& doc) {
            unsigned int tmp;
            InputUtils::fillFromJson(fieldName, tmp, doc);
            field = static_cast<ShipCategory>(tmp);
        }
        //Helping function overloading for Settings::ManeuverCalculation::ManeuverWay
        void fillFromJson(const char* fieldName, Settings::ManeuverCalculation::ManeuverWay& field, const rapidjson::Value& doc) {
            unsigned int tmp;
            InputUtils::fillFromJson(fieldName, tmp, doc);
            field = static_cast<Settings::ManeuverCalculation::ManeuverWay>(tmp);
        }
    }


    NavigationParameters::NavigationParameters(const rapidjson::Value& doc) {
        auto& fieldNames = navigationParametersFieldNames;

        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);

        fillFromJson(fieldNames.cat.c_str(), cat, doc);
        fillFromJson(fieldNames.lat.c_str(), lat, doc);
        fillFromJson(fieldNames.lon.c_str(), lon, doc);
        fillFromJson(fieldNames.SOG.c_str(), SOG, doc);
        fillFromJson(fieldNames.COG.c_str(), COG, doc);
        fillFromJson(fieldNames.STW.c_str(), STW, doc);
        fillFromJson(fieldNames.heading.c_str(), heading, doc);
        fillFromJson(fieldNames.width.c_str(), width, doc);
        fillFromJson(fieldNames.length.c_str(), length, doc);
        fillFromJson(fieldNames.width_offset.c_str(), width_offset, doc);
        fillFromJson(fieldNames.length_offset.c_str(), length_offset, doc);
        fillFromJson(fieldNames.timestamp.c_str(), timestamp, doc);
    }

    NavigationProblem::NavigationProblem(const rapidjson::Value& doc) {
        auto& fieldnames = vehicleParametersFieldNames;

        if (!doc.IsArray())
            throw std::runtime_error(strErrDocNotAnArray);

        for (auto& vehicleDoc : doc.GetArray()) {
            USV::VehicleParameters vehicle;

            if (!vehicleDoc.IsObject())
                throw std::runtime_error(strErrDocNotAnObject);
            fillFromJson(fieldnames.cat.c_str(), vehicle.cat, vehicleDoc);
            fillFromJson(fieldnames.id.c_str(), vehicle.id, vehicleDoc);
            fillFromJson(fieldnames.COG.c_str(), vehicle.COG, vehicleDoc);
            fillFromJson(fieldnames.lat.c_str(), vehicle.lat, vehicleDoc);
            fillFromJson(fieldnames.lon.c_str(), vehicle.lon, vehicleDoc);
            fillFromJson(fieldnames.SOG.c_str(), vehicle.SOG, vehicleDoc);
            fillFromJsonVolountary(fieldnames.width.c_str(), vehicle.width, vehicleDoc);
            fillFromJsonVolountary(fieldnames.length.c_str(), vehicle.length, vehicleDoc);
            fillFromJsonVolountary(fieldnames.width_offset.c_str(), vehicle.width_offset, vehicleDoc);
            fillFromJsonVolountary(fieldnames.length_offset.c_str(), vehicle.length_offset, vehicleDoc);
            fillFromJson(fieldnames.first_detect_dist.c_str(), vehicle.first_detect_dist, vehicleDoc);
            fillFromJsonVolountary(fieldnames.cross_dist.c_str(), vehicle.cross_dist, vehicleDoc);
            fillFromJson(fieldnames.timestamp.c_str(), vehicle.timestamp, vehicleDoc);
            vehicles.emplace_back(vehicle);
        }
    }

    FeatureCollection::FeatureCollection(const rapidjson::Value& doc) {
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);
        if (strcmp(doc.GetObject()["type"].GetString(), "FeatureCollection") != 0)
            throw std::runtime_error("document object should be FeatureCollection");

        const rapidjson::Value& featuresObject = doc["features"];
        assert(featuresObject.IsArray());
        for (auto& itr : featuresObject.GetArray()) {
            USV::Feature newFeature{(itr)};
            features.push_back(newFeature);
        }
    }

    Feature::Feature(const rapidjson::Value& doc) : geometry{(doc)["geometry"]} {
        auto& fieldnames = featurePropertiesFieldNames;

        auto& jsonProperties = (doc)["properties"];
        fillFromJsonVolountary(fieldnames.limitation_type.c_str(), properties.limitation_type, jsonProperties);
        fillFromJsonVolountary(fieldnames.distance.c_str(), properties.distance, jsonProperties);
        fillFromJsonVolountary(fieldnames.hardness.c_str(), properties.hardness, jsonProperties);
        fillFromJsonVolountary(fieldnames.id.c_str(), properties.id, jsonProperties);
        fillFromJsonVolountary(fieldnames.max_course.c_str(), properties.max_course, jsonProperties);
        fillFromJsonVolountary(fieldnames.max_speed.c_str(), properties.max_speed, jsonProperties);
        fillFromJsonVolountary(fieldnames.min_course.c_str(), properties.min_course, jsonProperties);
        fillFromJsonVolountary(fieldnames.source_id.c_str(), properties.source_id, jsonProperties);
        fillFromJsonVolountary(fieldnames.source_object_code.c_str(), properties.source_object_code, jsonProperties);
    }

    Geometry::Geometry(const rapidjson::Value& doc) {
        type = USV::GeometryTypeFromString(doc["type"].GetString());
        auto& jsonCoordinates = doc["coordinates"];
        switch (type) {
            case USV::GeometryPoint:
                // Our notation:
                // x -- lat, y -- lon
                // GeoJSON notation:
                // First coordinate -- lon, second -- lat
                coordinatesPoint = USV::Vector2(jsonCoordinates[1].GetDouble(),
                                                jsonCoordinates[0].GetDouble());
                
                break;
            case USV::GeometryLine:
                type = USV::GeometryLine;
                for (auto& itr1 : jsonCoordinates.GetArray())
                    // Our notation:
                    // x -- lat, y -- lon
                    // GeoJSON notation:
                    // First coordinate -- lon, second -- lat
                    coordinatesLine.emplace_back((itr1)[1].GetDouble(),
                                                 (itr1)[0].GetDouble());
                break;
            case USV::GeometryPolygon:
                type = USV::GeometryPolygon;
                for (auto& itr1 : jsonCoordinates.GetArray()) {
                    USV::Polygon newFeaturePolygon{};
                    for (auto& itr2 : (itr1).GetArray()) {
                        // Our notation:
                        // x -- lat, y -- lon
                        // GeoJSON notation:
                        // First coordinate -- lon, second -- lat
                        newFeaturePolygon.push_back(Vector2((itr2)[1].GetDouble(), (itr2)[0].GetDouble()));
                    }
                    coordinatesPolygon.push_back(newFeaturePolygon);
                }
                break;
            default:
                break;
        }
    }


    const struct {
        const strType wind_direction{ "wind_direction" };
        const strType wind_speed{ "wind_speed" };
        const strType tide_direction{ "tide_direction" };
        const strType tide_speed{ "tide_speed" };
        const strType swell{ "swell" };
        const strType visibility{ "visibility" };
    } HydrometeorologyFieldNames;


    Hydrometeorology::Hydrometeorology(const rapidjson::Value& doc) {
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);
        auto& fieldnames = HydrometeorologyFieldNames;

        fillFromJson(fieldnames.wind_direction.c_str(), wind_direction, doc);
        fillFromJson(fieldnames.wind_speed.c_str(), wind_speed, doc);
        fillFromJson(fieldnames.tide_direction.c_str(), tide_direction, doc);
        fillFromJson(fieldnames.tide_speed.c_str(), tide_speed, doc);
        fillFromJson(fieldnames.swell.c_str(), swell, doc);
        fillFromJson(fieldnames.visibility.c_str(), visibility, doc);
    }


    std::ostream& operator<<(std::ostream& os, const USV::Hydrometeorology& hydrometeorology) {
        os << HydrometeorologyFieldNames.wind_direction << ": " << hydrometeorology.wind_direction << std::endl;
        os << HydrometeorologyFieldNames.wind_speed << ": " << hydrometeorology.wind_speed << std::endl;
        os << HydrometeorologyFieldNames.tide_direction << ": " << hydrometeorology.tide_direction << std::endl;
        os << HydrometeorologyFieldNames.tide_speed << ": " << hydrometeorology.tide_speed << std::endl;
        os << HydrometeorologyFieldNames.swell << ": " << hydrometeorology.swell << std::endl;
        os << HydrometeorologyFieldNames.visibility << ": " << hydrometeorology.visibility;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const InputData& data) {
        if (data.navigationParameters)
            os << "navigationParameters: " << std::endl << *data.navigationParameters << std::endl;
        if (data.navigationProblem) os << "navigationProblem: " << *data.navigationProblem << std::endl;
        if (data.featureCollection) os << "featureCollection: " << std::endl << *data.featureCollection << std::endl;
        if (data.hydrometeorology) os << "hydrometeorology: " << std::endl << *data.hydrometeorology << std::endl;
        if (data.route) os << "route: " << std::endl << *data.route << std::endl;
        return os;
    }

    struct ManeuverCalculationFieldNames {
        static constexpr auto& priority{"priority"};
        static constexpr auto& maneuver_way{"maneuver_way"};
        static constexpr auto& safe_diverg_dist{"safe_diverg_dist"};
        static constexpr auto& min_diverg_dist{ "min_diverg_dist" };
        static constexpr auto& minimal_speed{"minimal_speed"};
        static constexpr auto& maximal_speed{"maximal_speed"};
        static constexpr auto& forward_speed1{"forward_speed1"};
        static constexpr auto& forward_speed2{"forward_speed2"};
        static constexpr auto& forward_speed3{"forward_speed3"};
        static constexpr auto& forward_speed4{"forward_speed4"};
        static constexpr auto& forward_speed5{"forward_speed5"};
        static constexpr auto& reverse_speed1{"reverse_speed1"};
        static constexpr auto& reverse_speed2{"reverse_speed2"};
        static constexpr auto& max_course_delta{"max_course_delta"};
        static constexpr auto& time_advance{"time_advance"};
        static constexpr auto& can_leave_route{"can_leave_route"};
        static constexpr auto& max_route_deviation{"max_route_deviation"};
        static constexpr auto& max_circulation_radius{"max_circulation_radius"};
        static constexpr auto& min_circulation_radius{"min_circulation_radius"};
        static constexpr auto& breaking_distance{"run_out_distance"};
        static constexpr auto& run_out_distance{"run_out_distance"};
        static constexpr auto& forecast_time{"forecast_time"};
    };

    const struct {
        const strType cpa{"cpa"};
        const strType tcpa{"tcpa"};
        const strType min_detect_dist{"min_detect_dist"};
        const strType last_moment_dist{"last_moment_dist"};
        const strType safety_zone{"safety_zone"};
    } safetyControlFieldNames;

    const struct {
        const strType safety_zone_type{ "safety_zone_type" };
        const strType radius{ "radius" };
        const strType start_angle{ "start_angle" };
        const strType end_angle{ "end_angle" };
        const strType length{ "length" };
        const strType width{ "width" };
        const strType lengthOffset{ "lengthOffset" };
    } safetyZoneFieldNames;


    Settings::Settings(const rapidjson::Value& doc) {
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);

        manuever_calculation = ManeuverCalculation{doc["maneuver_calculation"]};
        safety_control = SafetyControl{doc["safety_control"]};
    }

    Settings::ManeuverCalculation::ManeuverCalculation(const rapidjson::Value& doc) {
        using fn = ManeuverCalculationFieldNames;
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);
        fillFromJson(fn::priority, priority, doc);
        fillFromJson(fn::maneuver_way, maneuver_way, doc);
        fillFromJson(fn::safe_diverg_dist, safe_diverg_dist, doc);
        fillFromJson(fn::min_diverg_dist, min_diverg_dist, doc);
        fillFromJson(fn::minimal_speed, minimal_speed, doc);
        fillFromJson(fn::maximal_speed, maximal_speed, doc);
        fillFromJson(fn::forward_speed1, forward_speed1, doc);
        fillFromJson(fn::forward_speed2, forward_speed2, doc);
        fillFromJson(fn::forward_speed3, forward_speed3, doc);
        fillFromJson(fn::forward_speed4, forward_speed4, doc);
        fillFromJson(fn::forward_speed5, forward_speed5, doc);
        fillFromJson(fn::reverse_speed1, reverse_speed1, doc);
        fillFromJson(fn::reverse_speed2, reverse_speed2, doc);
        fillFromJson(fn::max_course_delta, max_course_delta, doc);
        fillFromJson(fn::time_advance, time_advance, doc);
        fillFromJson(fn::can_leave_route, can_leave_route, doc);
        fillFromJson(fn::max_route_deviation, max_route_deviation, doc);
        fillFromJson(fn::max_circulation_radius, max_circulation_radius, doc);
        fillFromJson(fn::min_circulation_radius, min_circulation_radius, doc);
        fillFromJson(fn::breaking_distance, breaking_distance, doc);
        fillFromJson(fn::run_out_distance, run_out_distance, doc);
        fillFromJson(fn::forecast_time, forecast_time, doc);
    }


    Settings::SafetyControl::SafetyControl(const rapidjson::Value& doc) {
        auto& fieldnames = safetyControlFieldNames;

        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);

        fillFromJson(fieldnames.cpa.c_str(), cpa, doc);
        fillFromJson(fieldnames.tcpa.c_str(), tcpa, doc);
        fillFromJson(fieldnames.min_detect_dist.c_str(), min_detect_dist, doc);
        fillFromJson(fieldnames.last_moment_dist.c_str(), last_moment_dist, doc);
        safety_zone = SafetyZone{doc[safetyControlFieldNames.safety_zone.c_str()]};
    }

    Settings::SafetyControl::SafetyZone::SafetyZone(const rapidjson::Value& doc) {
        if (!doc.IsObject()) {
            throw std::runtime_error(strErrDocNotAnObject);
        }
        auto& fieldnames = safetyZoneFieldNames;
        fillFromJson(fieldnames.safety_zone_type.c_str(), safety_zone_type, doc);
        switch (safety_zone_type) {
            case 0: {
                fillFromJson(fieldnames.radius.c_str(), radius, doc);
                break;
            }
            case 1: {
                fillFromJson(fieldnames.radius.c_str(), radius, doc);
                fillFromJson(fieldnames.start_angle.c_str(), start_angle, doc);
                fillFromJson(fieldnames.end_angle.c_str(), end_angle, doc);
                break;
            }
            case 2: {
                fillFromJson(fieldnames.length.c_str(), length, doc);
                fillFromJson(fieldnames.width.c_str(), width, doc);
                fillFromJson(fieldnames.lengthOffset.c_str(), lengthOffset, doc);
                break;
            }
            default: {
                throw std::runtime_error("Wrong zone type: " + std::to_string(safety_zone_type));
            }
        }
    }

    TargetSettings::TargetSettings(const rapidjson::Value& doc) {
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);

        manuever_calculation = ManeuverCalculation{doc["maneuver_calculation"]};
        safety_control = Settings::SafetyControl{doc["safety_control"]};
    }

    TargetSettings::TargetSettings(const Settings& settings) {
        manuever_calculation = ManeuverCalculation();
        // Fill maneuver_calculation:
        {
            manuever_calculation.safe_diverg_dist = settings.manuever_calculation.safe_diverg_dist;
            manuever_calculation.minimal_speed = settings.manuever_calculation.minimal_speed;
            manuever_calculation.maximal_speed = settings.manuever_calculation.maximal_speed;
            manuever_calculation.forward_speed1 = settings.manuever_calculation.forward_speed1;
            manuever_calculation.forward_speed2 = settings.manuever_calculation.forward_speed2;
            manuever_calculation.forward_speed3 = settings.manuever_calculation.forward_speed3;
            manuever_calculation.forward_speed4 = settings.manuever_calculation.forward_speed4;
            manuever_calculation.forward_speed5 = settings.manuever_calculation.forward_speed5;
            manuever_calculation.reverse_speed1 = settings.manuever_calculation.reverse_speed1;
            manuever_calculation.reverse_speed2 = settings.manuever_calculation.reverse_speed2;
            manuever_calculation.max_course_delta = settings.manuever_calculation.max_course_delta;
            manuever_calculation.time_advance = settings.manuever_calculation.time_advance;
            manuever_calculation.can_leave_route = settings.manuever_calculation.can_leave_route;
            manuever_calculation.max_route_deviation = settings.manuever_calculation.max_route_deviation;
            manuever_calculation.max_circulation_radius = settings.manuever_calculation.max_circulation_radius;
            manuever_calculation.min_circulation_radius = settings.manuever_calculation.min_circulation_radius;
            manuever_calculation.breaking_distance = settings.manuever_calculation.breaking_distance;
            manuever_calculation.run_out_distance = settings.manuever_calculation.run_out_distance;
            manuever_calculation.forecast_time = settings.manuever_calculation.forecast_time;
        }

        safety_control = settings.safety_control;
    }

    Settings::Settings(const TargetSettings& targetSettings) {
        manuever_calculation = ManeuverCalculation();
        // Fill maneuver_calculation:
        {
            manuever_calculation.priority = 0;
            manuever_calculation.maneuver_way = ManeuverCalculation::ManeuverWay::CourseAndSpeed;
            manuever_calculation.safe_diverg_dist = targetSettings.manuever_calculation.safe_diverg_dist;
            manuever_calculation.minimal_speed = targetSettings.manuever_calculation.minimal_speed;
            manuever_calculation.maximal_speed = targetSettings.manuever_calculation.maximal_speed;
            manuever_calculation.forward_speed1 = targetSettings.manuever_calculation.forward_speed1;
            manuever_calculation.forward_speed2 = targetSettings.manuever_calculation.forward_speed2;
            manuever_calculation.forward_speed3 = targetSettings.manuever_calculation.forward_speed3;
            manuever_calculation.forward_speed4 = targetSettings.manuever_calculation.forward_speed4;
            manuever_calculation.forward_speed5 = targetSettings.manuever_calculation.forward_speed5;
            manuever_calculation.reverse_speed1 = targetSettings.manuever_calculation.reverse_speed1;
            manuever_calculation.reverse_speed2 = targetSettings.manuever_calculation.reverse_speed2;
            manuever_calculation.max_course_delta = targetSettings.manuever_calculation.max_course_delta;
            manuever_calculation.time_advance = targetSettings.manuever_calculation.time_advance;
            manuever_calculation.can_leave_route = targetSettings.manuever_calculation.can_leave_route;
            manuever_calculation.max_route_deviation = targetSettings.manuever_calculation.max_route_deviation;
            manuever_calculation.max_circulation_radius = targetSettings.manuever_calculation.max_circulation_radius;
            manuever_calculation.min_circulation_radius = targetSettings.manuever_calculation.min_circulation_radius;
            manuever_calculation.breaking_distance = targetSettings.manuever_calculation.breaking_distance;
            manuever_calculation.run_out_distance = targetSettings.manuever_calculation.run_out_distance;
            manuever_calculation.forecast_time = targetSettings.manuever_calculation.forecast_time;
        }

        safety_control = targetSettings.safety_control;
    }

    TargetSettings::ManeuverCalculation::ManeuverCalculation(const rapidjson::Value& doc) {
        using fn = ManeuverCalculationFieldNames;
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);
        fillFromJson(fn::safe_diverg_dist, safe_diverg_dist, doc);
        fillFromJson(fn::minimal_speed, minimal_speed, doc);
        fillFromJson(fn::maximal_speed, maximal_speed, doc);
        fillFromJson(fn::forward_speed1, forward_speed1, doc);
        fillFromJson(fn::forward_speed2, forward_speed2, doc);
        fillFromJson(fn::forward_speed3, forward_speed3, doc);
        fillFromJson(fn::forward_speed4, forward_speed4, doc);
        fillFromJson(fn::forward_speed5, forward_speed5, doc);
        fillFromJson(fn::reverse_speed1, reverse_speed1, doc);
        fillFromJson(fn::reverse_speed2, reverse_speed2, doc);
        fillFromJson(fn::max_course_delta, max_course_delta, doc);
        fillFromJson(fn::time_advance, time_advance, doc);
        fillFromJson(fn::can_leave_route, can_leave_route, doc);
        fillFromJson(fn::max_route_deviation, max_route_deviation, doc);
        fillFromJson(fn::max_circulation_radius, max_circulation_radius, doc);
        fillFromJson(fn::min_circulation_radius, min_circulation_radius, doc);
        fillFromJson(fn::breaking_distance, breaking_distance, doc);
        fillFromJson(fn::run_out_distance, run_out_distance, doc);
        fillFromJson(fn::forecast_time, forecast_time, doc);
    }

    CurvedPathCollection::CurvedPathCollection(const rapidjson::Value& doc) {
        if (!doc.IsArray())
            throw std::runtime_error(strErrDocNotAnArray);

        for (auto& itr : doc.GetArray()) {
            USV::CurvedPath path{(itr)};
            paths.push_back(path);
        }
    }

    std::ostream& operator<<(std::ostream& os, const USV::CurvedPathCollection& cc) {
        size_t i = 0;
        for (auto& path:cc.paths) {
            os << "Path #" << i++ << std::endl;
            os << path << std::endl;
        }
        return os;
    }
}
