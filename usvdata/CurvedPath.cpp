#include "CurvedPath.h"
#include "InputUtils.h"

namespace USV {

    CurvedPath::CurvedPath(const rapidjson::Value& doc) {
        using namespace InputUtils;
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);

        InputUtils::fillFromJson(fieldnames::start_time, start_time, doc);

        auto itemsFileldName = fieldnames::items;
        if (!doc.HasMember(itemsFileldName))
            throw std::runtime_error(strErrMissingField + fieldnames::items);

        auto& itemsJson = doc.GetObject()[itemsFileldName];
        if (!itemsJson.IsArray())
            throw std::runtime_error(strErrDocNotAnArray);

        for (auto& itr : itemsJson.GetArray()) {
            Segment segment{itr};
            segments.push_back(segment);
        }
    }

    CurvedPath::Segment::Segment(const rapidjson::Value& doc) {
        using namespace InputUtils;
        if (!doc.IsObject())
            throw std::runtime_error(strErrDocNotAnObject);

        fillFromJson(fieldnames::lat, lat, doc);
        fillFromJson(fieldnames::lon, lon, doc);
        fillFromJson(fieldnames::begin_angle, begin_angle, doc);
        fillFromJson(fieldnames::curve, curve, doc);
        fillFromJson(fieldnames::length, length, doc);
        fillFromJson(fieldnames::duration, duration, doc);
        fillFromJson(fieldnames::port_dev, port_dev, doc);
        fillFromJson(fieldnames::starboard_dev, starboard_dev, doc);
    }

    std::ostream& operator<<(std::ostream& os, const USV::CurvedPath& curvedPath) {
        os << USV::CurvedPath::fieldnames::start_time << ": " << curvedPath.start_time << std::endl;
        os << USV::CurvedPath::fieldnames::items << ":" << std::endl;
        size_t i{0};
        for (auto& segment:curvedPath.segments) {
            os << "#" << i++ << ":" << std::endl;
            os << segment << std::endl;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const USV::CurvedPath::Segment& segment) {
        os << USV::CurvedPath::Segment::fieldnames::lat << ": " << segment.lat << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::lon << ": " << segment.lon << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::begin_angle << ": " << segment.begin_angle << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::curve << ": " << segment.curve << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::length << ": " << segment.length << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::duration << ": " << segment.duration << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::port_dev << ": " << segment.port_dev << std::endl;
        os << USV::CurvedPath::Segment::fieldnames::starboard_dev << ": " << segment.starboard_dev;
        return os;
    }
}