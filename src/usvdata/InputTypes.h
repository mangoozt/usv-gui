#ifndef USV_INPUTDATA_H
#define USV_INPUTDATA_H

#include "CurvedPath.h"
#include "Vector2.h"
#include "FeatureCollection.h"
#include <memory>
#include <string>
#include <fstream>
#include <ostream>

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
        double width_offset{
                0}; // Смещение точки измерения координат по ширине относительно левого борта. 0 если данные неизвестны. [miles]
        double length_offset{
                0}; // Смещение точки измерения координат по длине относительно кормы. может 0 если данные неизвестны. [miles]
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
            enum class ManeuverWay {
                Course = 0,
                Speed = 1,
                CourseAndSpeed = 2
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

    struct Maneuver {
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
        std::unique_ptr<Settings> settings;
        std::unique_ptr<Maneuvers> maneuvers;
        std::unique_ptr<CurvedPathCollection> targets_paths;
        std::unique_ptr<CurvedPathCollection> targets_real_paths;
        std::unique_ptr<FeatureCollection> constraints;
    };

}

#endif // USV_INPUTDATA_H
