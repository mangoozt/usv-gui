#ifndef USV_PATH_H
#define USV_PATH_H

#include "Angle.h"
#include "Vector2.h"
#include "CurvedPath.h"
#include "Frame.h"
#include <map>

namespace USV {
    class Path {
    private:
        double start_time;
    public:
        struct Position {
            Vector2 point;
            Angle course;
            double speed;
        };

        class Segment {

            Vector2 _start_point; //! координаты начала сегмента [miles]
            Angle _begin_angle; //! Начальный курс сегмента [radians]
            double _curve; //! Кривизна сегмента [1/miles]
            //! Модуль значения обратен радиусу поворота в милях. Знак значения определяет направление поворота(по phi)
            //! Нулевое значение кривизны кодирует линейный сегмент маршрута. Ненулевое – циркуляционный сегмент.
            double _length; //!  Длина сегмента [miles]
            double _duration; //! Длительность сегмента [sec]
            double _port_dev; //! Дистанция до левой границы сегмента [miles]
            double _starboard_dev; //! Дистанция до правой границы сегмента [miles]

            Vector2 _O_V;

            friend class Path;

        public:
            Segment(Vector2 start_point, Angle beginAngle, double curve, double length, double duration, double port_dev = 0, double starboard_dev = 0);

            Segment(Position start_position, double curve, double duration);

            /**
             * \brief Constructs Segment based on start position, curvature and angle. Sign of angle determines turning side. Start position determines speed of segment.
             * @param start_position position oa start of segment
             * @param curve Inverse of turning radius.
             * @param angle Turn angle
             */
            Segment(Position start_position, double curve, Angle angle);


            Position position(double t) const;

            /**
             * \brief Calculates closest distance from point to segment
             * @param point To calculate distance from
             * @return Distance to point
             */
            double distance(Vector2 point) const;

            /**
             * \brief Calculates closest distance from point to segment
             * @param point To calculate distance from
             * @return Distance to point
             */
            double distance_signed(Vector2 point) const;

            double lengthFromStart(Vector2 point) const;

            const Vector2& getStartPoint() const {
                return _start_point;
            }

            inline Angle getBeginAngle() const {
                return _begin_angle;
            }

            inline double getCurve() const {
                return _curve;
            }

            inline double getLength() const {
                return _length;
            }

            inline double getDuration() const {
                return _duration;
            }

            inline double getPortDev() const {
                return _port_dev;
            }

            inline double getStarboardDev() const {
                return _starboard_dev;
            }

            void cut(double t);

            inline Position end() const { return position(_duration); }
        };

        explicit Path(double startTime) : start_time(startTime) {}

        explicit Path(const CurvedPath& curved_path, const Frame& reference_frame);

        void appendSegment(Segment segment);

        Position position(double t) const;

        typedef std::map<double, Segment>::const_iterator constItr;

        constItr segment(double t) const;

        inline bool empty() const { return segments.empty(); };

        inline size_t size() const { return segments.size(); };

        inline constItr end() const { return segments.cend(); }

        Position endPosition();

        void cut(double t);

        inline double endTime() const { return (--segments.end())->first; }

        std::map<double, Segment> segments{};

        std::pair<double, constItr> closestSegment(Vector2 point) const;

        double getStartTime() const {
            return start_time;
        }


        //! \brief Look up for maximum distance between paths (regardless of time)
        //! \param path Second path
        //! \return true if distance within limit false otherwise
        bool checkMaxDistance(const Path& path, double limit) const;

        //! \brief Look up for maximum distance between paths
        //! \param path Second path
        //! \return maximum distance between paths
        double maxDistance(const Path& path) const;

    };

}
#endif //USV_PATH_H
