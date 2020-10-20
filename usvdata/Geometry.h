#ifndef USV_GEOMETRY_H
#define USV_GEOMETRY_H

#include "Vector2.h"
#include <vector>

namespace USV {
    typedef std::vector<USV::Vector2> Polygon;
    typedef std::vector<Polygon> MultiPolygon;

    class GeometryUtils {
    public:
        static bool PolygonsIntersects(const MultiPolygon& poly1, const MultiPolygon& poly2);

        static bool PointInMultiPolygon(const Vector2& point, const MultiPolygon& poly);

        static MultiPolygon PointToMultiPolygon(const Vector2& point, double width);

        static MultiPolygon LineToMultiPolygon(const std::vector<Vector2>& line, double width);

        static MultiPolygon
        SectorToMultiPolygon(const Vector2& point, double width, double angle1_degrees, double angle2_degrees);
    };

}
#endif //USV_GEOMETRY_H
