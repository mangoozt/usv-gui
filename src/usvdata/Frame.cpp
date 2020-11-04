#include "Frame.h"

namespace USV {
    using namespace GeographicLib;

    Vector2 Frame::fromWgs(double lat, double lon) const {
        auto wgs84 = Geodesic::WGS84();
        Math::real azi1, azi2, s12, angle, dist;
        wgs84.Inverse(_refLat, _refLon, lat, lon, s12, azi1, azi2);

        angle = azi1 * Math::degree();
        dist = s12 * invNauticalMile();
        return {dist * cos(angle), dist * sin(angle)};
    }

    void Frame::toWgs(const Vector2& vector, double& lat, double& lon) const {
        auto wgs84 = Geodesic::WGS84();
        auto azi1 = Math::atan2d(vector.y(), vector.x());
        //convert distance to meters
        auto distance = abs(vector) * Constants::nauticalmile();
        Math::real lat2, lon2;
        wgs84.Direct(_refLat, _refLon, azi1, distance, lat2, lon2);
        // return params
        lat = lat2;
        lon = lon2;
    }
}
