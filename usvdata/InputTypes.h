#ifndef USV_INPUTDATA_H
#define USV_INPUTDATA_H

#include "Geometry.h"
#include "Defines.h"
#include "CurvedPath.h"
#include <string>
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/istreamwrapper.h"
#include <fstream>
#include <ostream>

namespace USV {

    /**
     * \brief Навигационная обстановка собственного судна
     */
    struct NavigationParameters {
        NavigationParameters() = default;

        NavigationParameters(ShipCategory cat, double lat, double lon, double sog, double cog, double stw,
                             double heading, double width, double length, double widthOffset, double lengthOffset,
                             time_t timestamp) : cat(cat), lat(lat), lon(lon), SOG(sog), COG(cog), STW(stw),
                                                 heading(heading), width(width), length(length),
                                                 width_offset(widthOffset), length_offset(lengthOffset),
                                                 timestamp(timestamp) {}

        explicit NavigationParameters(const rapidjson::Value& doc);

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

    std::ostream& operator<<(std::ostream& os, const USV::NavigationParameters& np);

    /**
     * \brief Описание цели
     */
    struct VehicleParameters {
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

    bool operator==(const USV::VehicleParameters& a, const USV::VehicleParameters& b);

    std::ostream& operator<<(std::ostream& os, const USV::VehicleParameters& vp);

    class NavigationProblem {
    public:
        NavigationProblem() = default;

        explicit NavigationProblem(const rapidjson::Value& doc);

        std::vector<VehicleParameters> vehicles{};
    };

    std::ostream& operator<<(std::ostream& os, const USV::NavigationProblem& np);

    enum GeometryType {
        GeometryPoint,
        GeometryLine,
        GeometryPolygon
    };

    struct Geometry {
        explicit Geometry(const rapidjson::Value& doc);

        GeometryType type;
        std::vector<Polygon> coordinatesPolygon;
        std::vector<Vector2> coordinatesLine;
        Vector2 coordinatesPoint;
    };

    std::ostream& operator<<(std::ostream& os, const Geometry& geom);

    struct FeatureProperties {
        std::string id;
        std::string limitation_type;//!< Тип ограничения
        std::string hardness;//!< Жесткость ограничения. Может принимать значения «hard» или «soft».
        std::string source_id;//!< Идентифицирует сущность, из которой получено данное ограничение
        std::string source_object_code; //!< Обозначение (акроним) исходного объекта карты
        double distance; //!< Обязательный атрибут "distance", должен иметь значение дистанции в милях.
        double max_course;//!< Пара обязательных атрибутов "min_course", "max_course" задают границы допустимого значения курса в градусах (разрешаются все курсы от min_course по часовой стрелке до max_course).
        double min_course;
        double max_speed; //!< задает допустимое значение скорости в узлах [miles/hr].
    };

    struct Feature {
        explicit Feature(const rapidjson::Value& doc);

        Geometry geometry;
        FeatureProperties properties;
    };

    std::ostream& operator<<(std::ostream& os, const USV::Feature& feature);

    struct FeatureCollection {
        FeatureCollection() = default;

        explicit FeatureCollection(const rapidjson::Value& doc);

        std::vector<Feature> features;
    };

    std::ostream& operator<<(std::ostream& os, const USV::FeatureCollection& fc);

    GeometryType GeometryTypeFromString(const std::string& value);


    struct Hydrometeorology {
        Hydrometeorology() = default;

        double wind_direction{}; //! Направление ветра (абсолютное) [degrees]

        double wind_speed{}; //! Скорость ветра (абсолятная) [knots]

        double tide_direction{}; //! Направление течения (абсолютное) [degrees]

        double tide_speed{}; //! Скорость течения (абсолютное) [knots]

        double swell{}; //! Волнение

        double visibility{}; //! Дистанция видимости [nautical miles]

        Hydrometeorology(double windDirection, double windSpeed, double tideDirection,
                         double tideSpeed, double swell, double visibility) : wind_direction(windDirection),
                                                                              wind_speed(windSpeed),
                                                                              tide_direction(tideDirection),
                                                                              tide_speed(tideSpeed),
                                                                              swell(swell), visibility(visibility) {}

        explicit Hydrometeorology(const rapidjson::Value& doc);
    };

    std::ostream& operator<<(std::ostream& os, const USV::Hydrometeorology& hydrometeorology);

    struct TargetSettings;

    struct Settings {
        Settings() = default;

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

            ManeuverCalculation() = default;

            explicit ManeuverCalculation(const rapidjson::Value& doc);
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

                SafetyZone() = default;

                explicit SafetyZone(const rapidjson::Value& doc);

            } safety_zone; //End Class Safety_Zone


            SafetyControl() = default;

            explicit SafetyControl(const rapidjson::Value& doc);
        } safety_control; // End Class Safety_Control


        explicit Settings(const rapidjson::Value& doc);

        explicit Settings(const TargetSettings& target_settings);

    };

    struct TargetSettings {
        TargetSettings() = default;

        struct ManeuverCalculation {

            double safe_diverg_dist{}; //! Безопасная дистанция расхождения [miles]

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

            ManeuverCalculation() = default;

            explicit ManeuverCalculation(const rapidjson::Value& doc);
        } manuever_calculation; // End Class Maneuver_Calculation

        Settings::SafetyControl safety_control;

        explicit TargetSettings(const rapidjson::Value& doc);

        explicit TargetSettings(const Settings& settings);

    }; // End Class Settings

    struct CurvedPathCollection {
        std::vector<CurvedPath> paths;

        explicit CurvedPathCollection(const rapidjson::Value& doc);
    };

    std::ostream& operator<<(std::ostream& os, const CurvedPathCollection& cc);

    struct InputData {
        std::unique_ptr<NavigationParameters> navigationParameters;
        std::unique_ptr<NavigationProblem> navigationProblem;
        std::unique_ptr<FeatureCollection> featureCollection;
        std::unique_ptr<Hydrometeorology> hydrometeorology;
        std::unique_ptr<CurvedPath> route;
        std::unique_ptr<Settings> settings;
        std::unique_ptr<TargetSettings> target_settings;
    };

    std::ostream& operator<<(std::ostream& os, const USV::InputData& inputData);

}

#endif // USV_INPUTDATA_H
