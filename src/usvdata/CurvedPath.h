#ifndef USV_CURVEDPATH_H
#define USV_CURVEDPATH_H

#include "Defines.h"
#include <vector>
#include <ostream>
#include <string>

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

#endif //USV_CURVEDPATH_H
