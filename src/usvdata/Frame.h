#ifndef USV_FRAME_H_
#define USV_FRAME_H_

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
        [[nodiscard]] Vector2 fromWgs(double lat, double lon) const;

        /**
         * \brief		Convertes coordinates in reference frame to WGS84 coordinates.
         * \param       vector vector with coordinates in reference frame
         * \param		lat     latitude in WGS-84
         * \param		lon	    longitude in WGS-84
         */
        void toWgs(const Vector2& vector, double& lat, double& lon) const;

        inline double getRefLat() const {
            return _refLat;
        }

        inline double getRefLon() const {
            return _refLon;
        }

    private:
        double _refLat;
        double _refLon;
    };
}

#endif /* USV_FRAME_H_ */
