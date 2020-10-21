#ifndef USV_CURVEDPATH_H
#define USV_CURVEDPATH_H

#include "Defines.h"
#include <vector>
#include <ostream>
#include <string>

#include <spotify/json.hpp>

namespace USV {
class CurvedPath {
public:
    //! \class Segment
    class Segment {
    public:
        double lat{}; //! Широта начала сегмента [deg]
        //! >0 – северная широта, <0 – южная.
        double lon{}; //! Долгота [deg]
        //! >0 – восточная долгота, <0 – западная.
        double begin_angle{}; //! Начальный курс сегмента [deg]
        double curve{}; //! Кривизна сегмента [1/miles]
        //! Модуль значения обратен радиусу поворота в милях. Знак значения определяет направление поворота: < 0 – влево, > 0 – вправо.
        //! Нулевое значение кривизны кодирует линейный сегмент маршрута. Ненулевое – циркуляционный сегмент.
        double length{}; //!  Длина сегмента [miles]
        double duration{}; //! Длительность сегмента [sec]
        double port_dev{}; //! Дистанция до левой границы сегмента [miles]
        double starboard_dev{}; //! Дистанция до правой границы сегмента [miles]
    };

    std::vector<Segment> items; //! Элементы маршрута (сегменты)
    time_t start_time; //! Время начала маршрута в формате `yyyy.mm.dd-hh:mm:ss`

    CurvedPath() = default;

    explicit CurvedPath(time_t startTime) : start_time(startTime) {}

};
}

namespace spotify {
namespace json {
using namespace USV;
// Specialize spotify::json::default_codec_t to specify default behavior when
// encoding and decoding objects of certain types.
template <>
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
}

#endif //USV_CURVEDPATH_H
