#ifndef USV_GUI_FEATURECOLLECTION_H
#define USV_GUI_FEATURECOLLECTION_H

#include <vector>

namespace USV {
    enum class LimitationType {
        point_approach_prohibition,
        line_crossing_prohibition,
        zone_entering_prohibition,
        zone_leaving_prohibition,
        movement_parameters_limitation
    };

    enum class RestrictionType {
        Hard,
        Soft
    };

    enum class GeometryType {
        GeometryPoint,
        GeometryLine,
        GeometryPolygon
    };

    struct FeatureGeometry {
        GeometryType type;
        std::vector<std::vector<Vector2>> coordinatesPolygon;
        std::vector<Vector2> coordinatesLine;
        Vector2 coordinatesPoint;
    };


    struct FeatureProperties {
        std::string id;
        LimitationType limitation_type;//!< Тип ограничения
        RestrictionType hardness;//!< Жесткость ограничения. Может принимать значения «hard» или «soft».
        std::string source_id;//!< Идентифицирует сущность, из которой получено данное ограничение
        std::string source_object_code; //!< Обозначение (акроним) исходного объекта карты
        double distance; //!< Обязательный атрибут "distance", должен иметь значение дистанции в милях.
        double max_course;//!< Пара обязательных атрибутов "min_course", "max_course" задают границы допустимого значения курса в градусах (разрешаются все курсы от min_course по часовой стрелке до max_course).
        double min_course;
        double max_speed; //!< задает допустимое значение скорости в узлах [miles/hr].
    };

    struct Feature {
        FeatureGeometry geometry;
        FeatureProperties properties;
    };

    struct FeatureCollection {
        std::vector<Feature> features;
    };

}
#endif //USV_GUI_FEATURECOLLECTION_H
