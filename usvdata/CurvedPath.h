#ifndef USV_CURVEDPATH_H
#define USV_CURVEDPATH_H

#include "Defines.h"
#include "include/rapidjson/pointer.h"
#include <vector>
#include <ostream>
#include <string>

namespace USV {
    class CurvedPath {
    public:
        struct fieldnames{
            static constexpr auto& items{"items"};
            static constexpr auto& start_time{"start_time"};
        };
        //! \class Segment
        class Segment {
        public:
            struct fieldnames{
                static constexpr auto& lat{"lat"};
                static constexpr auto& lon{"lon"};
                static constexpr auto& begin_angle{"begin_angle"};
                static constexpr auto& curve{"curve"};
                static constexpr auto& length{"length"};
                static constexpr auto& duration{"duration"};
                static constexpr auto& port_dev{"port_dev"};
                static constexpr auto& starboard_dev{"starboard_dev"};
            };

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

            Segment(double lat, double lon, double beginAngle, double curve, double length, double duration,
                    double portDev, double starboardDev) : lat(lat), lon(lon), begin_angle(beginAngle), curve(curve),
                                                           length(length), duration(duration), port_dev(portDev),
                                                           starboard_dev(starboardDev) {}

            explicit Segment(const rapidjson::Value& doc);
        };

        std::vector<Segment> segments; //! Элементы маршрута (сегменты)
        time_t start_time; //! Время начала маршрута в формате `yyyy.mm.dd-hh:mm:ss`

        explicit CurvedPath(const rapidjson::Value& doc);

        explicit CurvedPath(time_t startTime) : start_time(startTime) {}

    };

    std::ostream& operator<<(std::ostream& os, const USV::CurvedPath& curvedPath);

    std::ostream& operator<<(std::ostream& os, const USV::CurvedPath::Segment& segment);

}

#endif //USV_CURVEDPATH_H
