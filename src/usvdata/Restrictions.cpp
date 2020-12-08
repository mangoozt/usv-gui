#include "Restrictions.h"

#include <utility>
#include "InputTypes.h"

namespace USV::Restrictions {
    namespace {
        inline Vector2 geoJSONToLocal(const Vector2& point, const USV::Frame& reference_frame) {
            return reference_frame.fromWgs(point.y(), point.x());
        }

        bool pointInRing(const ring_type& ring, const Vector2& point) {
            bool c = false;
            for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
                if (((ring[i].y() > point.y()) != (ring[j].y() > point.y())) &&
                    (point.x() <
                     (ring[j].x() - ring[i].x()) * (point.y() - ring[i].y()) /
                     (ring[j].y() - ring[i].y()) + ring[i].x()))
                    c = !c;
            }
            return c;
        };

        bool pointInPolygon(const Polygon& polygon, const Vector2& point) {
            if (pointInRing(polygon.rings[0], point)) {
                size_t c = 1;
                for (size_t i = 1; i < polygon.rings.size(); ++i) {
                    if (pointInRing(polygon.rings[i], point))
                        ++c;
                }
                return c % 2 == 1;
            }
            return false;
        }

        bool clockwiseRing(const ring_type& ring) {
            double sum = 0;
            const std::size_t len = ring.size();
            std::size_t i, j;
            for (i = 0, j = len > 0 ? len - 1 : 0; i < len; j = i++) {
                const auto& p1 = ring[i];
                const auto& p2 = ring[j];
                sum += (p2.x() - p1.x()) * (p1.y() + p2.y());
            }
            return (sum > 0);
        }
    }

    LineString lineToLocal(const std::vector<Vector2>& line, const USV::Frame& reference_frame) {
        LineString linestring;
        linestring.reserve(line.size());
        for (auto& point: line)
            linestring.push_back(geoJSONToLocal(point, reference_frame));
        return linestring;
    }

    Polygon polygonToLocal(const std::vector<std::vector<Vector2>>& polygon, const Frame& frame) {
        Polygon poly;
        poly.rings.reserve(polygon.size());
        for (const auto& itr : polygon) {
            poly.rings.emplace_back(0);
            auto& ring = poly.rings.back();
            ring.reserve(itr.size());
            for (auto& point: itr)
                ring.push_back(geoJSONToLocal(point, frame));
            ring.pop_back();
        }
        if (clockwiseRing(poly.rings[0]))
            std::reverse(poly.rings[0].begin(), poly.rings[0].end());
        return poly;
    }

    USV::Restrictions::Restrictions::Restrictions(const USV::InputTypes::FeatureCollection& feature_collection,
                                                  const USV::Frame& reference_frame) {
        for (auto& feature:feature_collection.features) {
            properties.push_back(feature.properties);
            auto& proper = feature.properties.hardness == RestrictionType::Soft ? soft : hard;
            switch (feature.properties.limitation_type) {
                case LimitationType::point_approach_prohibition: {
                    auto point = geoJSONToLocal(feature.geometry.coordinatesPoint, reference_frame);
                    proper.add_point_approach_prohibition(point, &properties.back());
                }
                    break;
                case LimitationType::line_crossing_prohibition: {
                    auto line = lineToLocal(feature.geometry.coordinatesLine, reference_frame);
                    proper.add_line_crossing_prohibition(line, &properties.back());
                }
                    break;
                case LimitationType::zone_entering_prohibition: {
                    Polygon poly = polygonToLocal(feature.geometry.coordinatesPolygon, reference_frame);
                    // Check if we within outer ring
                    proper.add_zone_entering_prohibition(poly, &properties.back());
                }
                    break;
                case LimitationType::zone_leaving_prohibition: {
                    Polygon poly = polygonToLocal(feature.geometry.coordinatesPolygon, reference_frame);
                    // Add only zones where we are already
                    proper.add_zone_leaving_prohibition(poly, &properties.back());
                }
                    break;
                case LimitationType::movement_parameters_limitation: {
                    Polygon poly = polygonToLocal(feature.geometry.coordinatesPolygon, reference_frame);
                    proper.add_movement_parameters_limitation(poly, &properties.back());
                }
                    break;
            }
        }
    }

    void Limitations::add_point_approach_prohibition(Vector2 point, FeatureProperties* features_ptr) {
        point_approach_prohibitions.push_back({point, features_ptr});
    }

    void Limitations::add_line_crossing_prohibition(LineString& linestring, FeatureProperties* features_ptr) {
        line_crossing_prohibitions.push_back({std::move(linestring), features_ptr});
    }

    void Limitations::add_zone_entering_prohibition(Polygon& polygon, FeatureProperties* features_ptr) {
        if (!pointInPolygon(polygon, {0, 0}))
            zone_entering_prohibitions.push_back({polygon, features_ptr});
    }

    void Limitations::add_zone_leaving_prohibition(Polygon& polygon, FeatureProperties* features_ptr) {
        if (pointInPolygon(polygon, {0, 0}))
            zone_leaving_prohibitions.push_back({polygon, features_ptr});
    }

    void Limitations::add_movement_parameters_limitation(Polygon& polygon, FeatureProperties* features_ptr) {
        movement_parameters_limitations.push_back({polygon, features_ptr});
    }
}