#ifndef USV_INPUTDATA_H
#define USV_INPUTDATA_H

#include "Defines.h"
#include "CurvedPath.h"
#include <string>
#include <fstream>
#include <ostream>

#include <spotify/json.hpp>

namespace USV::InputTypes {

/**
     * \brief Навигационная обстановка собственного судна
     */
struct NavigationParameters {
    ShipCategory cat{}; // Категория собственного судна в контексте правила 18 МППСС-72.

    double lat{}; // Широта собственного судна (WGS-84)	Значение с плавающей точкой в градусах. >0 – северная широта, <0 – южная.
    double lon{}; // Долгота собственного судна (WGS-84) Значение с плавающей точкой в градусах. >0 – восточная долгота, <0 – западная.
    double SOG{}; // Скорость собственного судна относительно грунта (путевая скорость)	Значение с плавающей точкой в узлах.
    double COG{}; // Курс собственного судна относительно грунта (путевой угол)	Значение с плавающей точкой в градусах.
    double STW{}; // Скорость собственного судна относительно воды	Значение с плавающей точкой в узлах
    double heading{}; // Компасный курс собственного судна	Значение с плавающей точкой в градусах.
    double width{}; // Ширина судна [meters]
    double length{}; // Длина судна [meters]
    double width_offset{}; // Смещение точки измерения координат по ширине относительно левого борта [meters]
    double length_offset{}; // Смещение точки измерения координат по длине относительно кормы [meters]
    time_t timestamp{}; // Момент времени (UTC), которому соответствуют остальные параметры	Строка в формате HH-MM-SS.

};

/**
     * \brief Описание цели
     */
struct TargetParameters {
    std::string id; // Идентификатор цели	Строка
    ShipCategory cat; // Категория судна в контексте правила 18 МППСС-72.

    double lat; // Широта цели (WGS-84)	Значение с плавающей точкой в градусах. > 0 – северная широта, < 0 – южная.
    double lon; // Долгота цели (WGS-84)	Значение с плавающей точкой в градусах. >0 – восточная долгота, <0 – западная.
    double SOG; // Скорость цели относительно грунта	Значение с плавающей точкой в узлах
    double COG; // Курс цели относительно грунта	Значение с плавающей точкой в градусах.
    double width{0}; // Ширина судна. 0 если данные неизвестны. [meters]
    double length{0}; // Длина судна. 0 если данные неизвестны. [meters]
    double width_offset{0}; // Смещение точки измерения координат по ширине относительно левого борта. 0 если данные неизвестны. [miles]
    double length_offset{0}; // Смещение точки измерения координат по длине относительно кормы. может 0 если данные неизвестны. [miles]
    double first_detect_dist; // Дистанция первого обнаружения. [miles]
    double cross_dist{0}; // Дистанция до пересечения курса. Отсутствует, если цель не пересекает курс. [miles]
    time_t timestamp; // Момент времени (UTC), которому соответствуют остальные параметры. Число секунд, прошедших с 00:00:00 01.01.1970 (UTC)
};

typedef std::vector<TargetParameters> TargetsParameters;

struct Hydrometeorology {
    double wind_direction{}; //! Направление ветра (абсолютное) [degrees]
    double wind_speed{}; //! Скорость ветра (абсолятная) [knots]
    double tide_direction{}; //! Направление течения (абсолютное) [degrees]
    double tide_speed{}; //! Скорость течения (абсолютное) [knots]
    double swell{}; //! Волнение
    double visibility{}; //! Дистанция видимости [nautical miles]
};

struct Settings {
    struct ManeuverCalculation {

        int priority{}; //! Приоритет в расчетах 0 - минимальное время выполения маневра;
        //! 1 - минимальное отклонение от маршрута
        //! 2 - минимальная скорость
        //! Способ выполнения маневра 0 - маневр курсом, 1 - маневр скоростью, 2 - маневром курсом и скоростью.
        enum class ManeuverWay{
            Course = 0,
            Speed = 1,
            CourseAndSpeed =2
        };
        ManeuverWay maneuver_way{};

        double safe_diverg_dist{}; //! Безопасная дистанция расхождения [miles]

        double min_diverg_dist{}; //! Минимальная допустимая дистанция расхождения [miles]

        double minimal_speed{}; //! Минимальная допустимая скорость [miles / h]

        double maximal_speed{}; //! Максимальная допустимая скорость [miles / h]

        double forward_speed1{}; //! Скорость ступени 1 (самая низкая) [miles / h]
        double forward_speed2{}; //! Скорость ступени 2 [miles / h]
        double forward_speed3{}; //! Скорость ступени 3 [miles / h]
        double forward_speed4{}; //! Скорость ступени 4 [miles / h]
        double forward_speed5{}; //! Скорость ступени 5 (самая высокая) [miles / h]

        double reverse_speed1{}; //! Скорость заднего хода 1 [miles / h]
        double reverse_speed2{}; //! Скорость заднего хода 2 [miles / h]

        double max_course_delta{}; //! Максимальное отклонение по курсу от оси маршрута [miles]

        double time_advance{}; //! Время упреждения [sec]

        bool can_leave_route{}; //! Разрешено ли выходить за границы маршрута

        double max_route_deviation{}; //! Максимальное отклоенние от оси маршрута [miles]

        double max_circulation_radius{}; //! Радиус циркуляции, максимальный [miles]

        double min_circulation_radius{}; //! Радиус циркуляции, минимальный [miles]

        double breaking_distance{}; //! Тормозной путь [miles]

        double run_out_distance{}; //! Дистанция выбега [miles]

        double forecast_time{}; //! Время прогнозирования развития обстановки [sec]
    } manuever_calculation; // End Class Maneuver_Calculation


    struct SafetyControl {

        double cpa{}; //! Допустимая дистанция кратчайшего сближения [miles]
        double tcpa{}; //! Допустимое время до точки кратчайшего сближения, нарушающего заданное
        //! значение tcpa [sec]
        double min_detect_dist{}; //! минимальная дистация обнаружения [miles]
        double last_moment_dist{}; //! Дистанция последнего момента [miles]

        struct SafetyZone {

            int safety_zone_type{0}; //! Тип зоны безопасности 0 - круг, 1 - сектор, 2 - прямоугольник

            double radius{}; //! Радиус круглой зоны безопасности [miles]
            double start_angle{}; //! Начальный угол (пеленг) [degrees]
            double end_angle{}; //! Конечный угол (пеленг) [degrees]

            double length{}; //! Длина прямоугольной зоны безопасности [miles]
            double width{}; //! Ширина прямоугольной зоны безопасности [miles]
            double lengthOffset{}; //! Смещение центра зоны относительно центра судна по длине
            //! (в сторону носа) [miles]
        } safety_zone; //End Class Safety_Zone
    } safety_control; // End Class Safety_Control
};

typedef std::vector<CurvedPath> CurvedPathCollection;

struct Maneuver{
    enum class SolutionType {
        Solved = 0,
        SolvedWithViolations,
        NotSolved
    };

    SolutionType solution_type;
    CurvedPath path;
    std::string msg;
    std::string solver_name;
};

typedef std::vector<Maneuver> Maneuvers;

struct InputData {
    std::unique_ptr<NavigationParameters> navigationParameters;
    std::unique_ptr<TargetsParameters> navigationProblem;
    std::unique_ptr<Hydrometeorology> hydrometeorology;
    std::unique_ptr<CurvedPath> route;
    //        std::unique_ptr<Settings> settings;
    std::unique_ptr<Maneuvers> maneuvers;
    std::unique_ptr<CurvedPathCollection> targets_paths;
};

}

namespace spotify {
namespace json {
using namespace USV::InputTypes;
// Specialize spotify::json::default_codec_t to specify default behavior when
// encoding and decoding objects of certain types.

template <>
struct default_codec_t<ShipCategory> {
    static int CatDecode(const ShipCategory cat){
        return static_cast<int>(cat);
    }

    static ShipCategory CatEncode(const int cat){
        return static_cast<ShipCategory>(cat);
    }

  static auto codec() {
//    auto codec = codec::enumeration();
    auto codec = codec::transform(codec::number<int>(),CatDecode,CatEncode);
    return codec;
  }
};

template <>
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
    static int SolutionTypeDecode(const Maneuver::SolutionType cat){
        return static_cast<int>(cat);
    }

    static Maneuver::SolutionType SolutionTypeEncode(const int cat){
        return static_cast<Maneuver::SolutionType>(cat);
    }

    static codec::object_t<Maneuver> codec() {
        auto codec = codec::object<Maneuver>();
        auto type_codec = codec::transform(codec::number<int>(),SolutionTypeDecode,SolutionTypeEncode);
        codec.required("solution_type", &Maneuver::solution_type, type_codec);
        codec.required("path", &Maneuver::path);
        codec.required("msg", &Maneuver::msg);
        codec.required("solver_name", &Maneuver::solver_name);
        return codec;
    }
};

}
}
#endif // USV_INPUTDATA_H
