#ifndef USV_GUI_RESTRICTIONS_H
#define USV_GUI_RESTRICTIONS_H

#include "Vector2.h"
#include "Frame.h"
#include <vector>
#include <deque>

namespace USV::InputTypes {
    struct FeatureCollection;
}

namespace USV::Restrictions {

    typedef std::vector<Vector2> LineString;
    typedef std::vector<Vector2> ring_type;

    struct Polygon {
        std::vector<ring_type> rings;
    };

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

    class Limitations {
    public:
        struct Limitation {
            struct point_approach_prohibition {
                Vector2 point;
                FeatureProperties* _ptr{};
            };
            struct line_crossing_prohibition {
                LineString linestring;
                FeatureProperties* _ptr;
            };
            struct zone_entering_prohibition {
                Polygon polygon;
                FeatureProperties* _ptr{};
            };
            struct zone_leaving_prohibition {
                Polygon polygon;
                FeatureProperties* _ptr{};
            };
            struct movement_parameters_limitation {
                Polygon polygon;
                FeatureProperties* _ptr{};
            };
        };
    private:
        bool empty_{true};
        std::vector<Limitation::point_approach_prohibition> point_approach_prohibitions;
        std::vector<Limitation::line_crossing_prohibition> line_crossing_prohibitions;
        std::vector<Limitation::zone_entering_prohibition> zone_entering_prohibitions;
        std::vector<Limitation::zone_leaving_prohibition> zone_leaving_prohibitions;
        std::vector<Limitation::movement_parameters_limitation> movement_parameters_limitations;
    public:
        Limitations() = default;

        void add_point_approach_prohibition(Vector2 point, FeatureProperties*);

        void add_line_crossing_prohibition(LineString& linestring, FeatureProperties*);

        void add_zone_entering_prohibition(Polygon& polygon, FeatureProperties*);

        void add_zone_leaving_prohibition(Polygon& polygon, FeatureProperties*);

        void add_movement_parameters_limitation(Polygon& polygon, FeatureProperties* features_ptr);

        [[nodiscard]] const std::vector<Limitation::point_approach_prohibition>& PointApproachProhibitions() const {
            return point_approach_prohibitions;
        }

        [[nodiscard]] const std::vector<Limitation::line_crossing_prohibition>& LineCrossingProhibitions() const {
            return line_crossing_prohibitions;
        }

        [[nodiscard]] const std::vector<Limitation::zone_entering_prohibition>& ZoneEnteringProhibitions() const {
            return zone_entering_prohibitions;
        }

        [[nodiscard]] const std::vector<Limitation::zone_leaving_prohibition>& ZoneLeavingProhibitions() const {
            return zone_leaving_prohibitions;
        }

        [[nodiscard]] const std::vector<Limitation::movement_parameters_limitation>&
        MovementParametersLimitations() const {
            return movement_parameters_limitations;
        }

        [[nodiscard]] bool empty() const {
            return empty_;
        }
    };

    struct Restrictions {
        Limitations hard;
        Limitations soft;

        std::deque<FeatureProperties> properties{};

        Restrictions() = default;

        explicit Restrictions(const USV::InputTypes::FeatureCollection& feature_collection,
                              const USV::Frame& reference_frame);

        [[nodiscard]] bool empty() const {
            return hard.empty() && soft.empty();
        }


    };

    Polygon polygonToLocal(const std::vector<std::vector<Vector2>>& polygon, const Frame& frame);

    LineString lineToLocal(const std::vector<Vector2>& line, const USV::Frame& reference_frame);

};

#endif //USV_GUI_RESTRICTIONS_H
