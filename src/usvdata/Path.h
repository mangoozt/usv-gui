#ifndef USV_PATH_H
#define USV_PATH_H

#include "Angle.h"
#include "Vector2.h"
#include "CurvedPath.h"
#include "Frame.h"

namespace USV {
    class Path {
    private:
        double start_time;
    public:
        struct Position {
            Vector2 point;
            Angle course;
            double speed{0.0};
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

            Vector2 O_V;

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


            [[nodiscard]] Position position(double t) const;

            /**
             * \brief Calculates closest distance from point to segment
             * @param point To calculate distance from
             * @return Distance to point
             */
            [[nodiscard]] double distance(Vector2 point) const;

            /**
             * \brief Calculates closest distance from point to segment
             * @param point To calculate distance from
             * @return Distance to point
             */
            [[nodiscard]] double distance_signed(Vector2 point) const;

            [[nodiscard]] double lengthFromStart(Vector2 point) const;

            [[nodiscard]] const Vector2& getStartPoint() const {
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

            [[nodiscard]] inline Position end() const { return position(_duration); }
        };

        explicit Path(double startTime) : start_time(startTime) {}

        explicit Path(const CurvedPath& curved_path, const Frame& reference_frame);

        typedef std::vector<std::pair<double, Segment>> SegmentsType;

        typedef SegmentsType::const_iterator constItr;

    private:
        SegmentsType segments{};
    public:
        void appendSegment(Segment segment);

        [[nodiscard]] const SegmentsType &getSegments() const;

        [[nodiscard]] Position position(double t) const;

        [[nodiscard]] constItr segment(double t) const;

        [[nodiscard]] inline constItr end() const { return segments.cend(); }

        [[nodiscard]] inline bool empty() const { return segments.empty(); };

        [[nodiscard]] inline size_t size() const { return segments.size(); };

        [[nodiscard]] Position endPosition() const;

        void cut(double t);

        [[nodiscard]] inline double endTime() const { return (--segments.end())->first; }

        [[nodiscard]] std::pair<double, constItr> closestSegment(Vector2 point) const;

        [[nodiscard]] inline double getStartTime() const {
            return start_time;
        }

        [[nodiscard]] std::vector<Vector2> getPointsPath(double angle_increment=2.0/180*M_PI)const;

    };

}
#endif //USV_PATH_H
