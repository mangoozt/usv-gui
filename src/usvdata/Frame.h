#ifndef USV_FRAME_H_
#define USV_FRAME_H_

#include <GeographicLib/Geodesic.hpp>
#include "Vector2.h"

namespace USV {
    /**
     * \brief      Defines converter class
     */
    class Frame {
    public:
        /**
         * \brief		Constructs a frame instance.
         * \param		lat		Reference point latitude
         * \param		lon		Reference point longitude
         */
        Frame(double lat, double lon) : _refLat(lat), _refLon(lon) {};

        /**
         * \brief		Converts WGS84 coordinates to coordinates in reference frame using.
         * \param		lat		latitude
         * \param		lon		longitude
         */
        Vector2 fromWgs(double lat, double lon) const;

        /**
         * \brief		Convertes coordinates in reference frame to WGS84 coordinates.
         * \param       vector vector with coordinates in reference frame
         * \param		lat     latitude in WGS-84
         * \param		lon	    longitude in WGS-84
         */
        void toWgs(const Vector2& vector, double& lat, double& lon) const;

    private:
        double _refLat;
        double _refLon;

        static inline GeographicLib::Math::real invNauticalMile() {
            return 1 / GeographicLib::Constants::nauticalmile();
        }

        static inline GeographicLib::Math::real invDegree() { return 1 / GeographicLib::Math::degree(); }
    };
}

#endif /* USV_FRAME_H_ */
