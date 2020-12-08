#include "Path.h"

namespace USV {

    Path::Position USV::Path::Segment::position(double t) const {
        if (t < 0 || t - _duration > 0.01) {
            throw std::out_of_range("Time exceeds duration of segment");
        }
        if (t > _duration) t = _duration;
        double speed = _length / _duration;
        auto course = _begin_angle;
        Vector2 delta;
        if (0.0000001 < std::abs(_curve)) {
            double r = 1 / _curve;
            double alpha = speed * t * _curve;
            auto negphi = (-O_V).phi();
            auto d = Vector2::polar(std::abs(r), negphi + alpha);
            delta = O_V + d;
            course += alpha;
        } else {
            delta = t * O_V;
        }
        return {_start_point + delta, course, speed};
    }

    void Path::Segment::cut(double t) {
        _length = _length / _duration * t;
        _duration = t;
    }

    double Path::Segment::distance(Vector2 C) const {
        return std::abs(distance_signed(C));
    }

    double Path::Segment::distance_signed(Vector2 C) const {
        if (0.0000001 > std::abs(_curve)) {
            auto& A = _start_point;
            auto B = _start_point + Vector2::polar(_length, _begin_angle.radians());
            // Straight
            /*
                                     AC * AB
                                r = ---------
                                      |AC|
                    r has the following meaning:
                    1)
                        r = 0: P = A
                        r < 0: P is on the backward extension of AB
                    2)
                        r = |AB|: P = B
                        r > |AB|: P is on the forward extension of AB
                    3)
                        0 < r < |AB|: P is interior to AB
                */
            auto AB = B - A;
            auto AC = C - A;
            auto ABabs = abs(AB);
            auto r = AC * AB / ABabs;
            // 1)
            if (r <= 0.0) {
                return abs(AC);
            }
            // 2)
            if (r >= _length) {
                return abs(C - B);
            }
            /*
                    3)
                                    |AB^AC|
                        distance = ---------
                                       L
                */
            return det(AB, AC) / _length;
        } else {
            double r = 1 / _curve;


            Vector2 O = Vector2::polar(r, (_begin_angle + M_PI_2).radians()) + _start_point;
            auto OC = C - O;

            auto gamma = Angle(OC.phi());
            Angle alpha;
            if (r > 0)
                alpha = Angle(_begin_angle - M_PI_2);
            else
                alpha = Angle(_begin_angle + M_PI_2);
            auto beta = alpha + Angle(_length * _curve);

            auto d1 = (gamma - alpha).radians();
            auto d2 = (gamma - beta).radians();
            if ((r > 0 && (d1 >= 0 && d2 <= 0)) || (r < 0 && (d1 <= 0 && d2 >= 0))) {
                // On arch
                return std::abs(std::abs(r) - abs(OC));
            }
            if (std::abs(d1) < std::abs(d2)) {
                // start_point closer
                return abs(_start_point - C);
            } else {
                // end point closer
                // Using Law of cosines
                auto OCSqr = absSq(OC);
                return std::sqrt(r * r + OCSqr - 2 * std::abs(r) * std::sqrt(OCSqr) * std::cos(std::abs(d2)));
            }
        }
    }

    Path::Segment::Segment(Path::Position start_position, double curve, Angle angle) :
            _start_point(start_position.point), _begin_angle(start_position.course) {

        curve = std::abs(curve);
        if (curve < 0.0000001) throw std::runtime_error("Zero curvature");
        _length = std::abs(angle.radians()) / curve;
        _duration = _length / start_position.speed;
        _curve = (angle.radians() < 0) ? -curve : curve;
        _port_dev = 0;
        _starboard_dev = 0;
        if (std::isinf(_duration) || std::isnan(_duration))
            throw std::runtime_error("Non-normal duration value");
        if (_duration < 0)
            throw std::runtime_error("Negative duration");
        if (std::isinf(_length) || std::isnan(_length))
            throw std::runtime_error("Non-normal length value");
        if (_length < 0)
            throw std::runtime_error("Negative length value");
        if (0.0000001 < std::abs(_curve)) {
            double r = 1 / _curve;
            O_V = Vector2::polar(r, (_begin_angle + M_PI * 0.5).radians());
        } else {
            double speed = _length / _duration;
            O_V = Vector2::polar(speed, _begin_angle.radians());
        }
    }

    double Path::Segment::lengthFromStart(Vector2 C) const {
        if (0.0000001 > std::abs(_curve)) {
            auto AC = C - _start_point;
            return AC * Vector2::polar(1, _begin_angle);
        } else {
            double r = 1 / _curve;

            Vector2 AO = Vector2::polar(r, (_begin_angle + M_PI_2).radians());
            auto AC = C - _start_point;

            /*
                 *            AC^AO
                 *   alpha = -------
                 *            OC.AO
                 */
            auto alpha = std::fmod(std::asin(det(AC, AO) / (abs(AC - AO) * abs(AO))) * (_curve > 0 ? 1 : -1) + M_2PI,
                                   M_2PI);

            return alpha * std::abs(r);
        }
    }

    Path::Segment::Segment(Vector2 start_point, Angle beginAngle, double curve, double length, double duration,
                           double port_dev, double starboard_dev) :
            _start_point(start_point), _begin_angle(beginAngle), _curve(curve), _length(length), _duration(
            duration), _port_dev(port_dev), _starboard_dev(starboard_dev) {
        if (std::isinf(duration) || std::isnan(duration))
            throw std::runtime_error("Non-normal duration value");

        if (0.0000001 < std::abs(_curve)) {
            double r = 1 / _curve;
            O_V = Vector2::polar(r, (_begin_angle + M_PI * 0.5).radians());
        } else {
            double speed = _length / _duration;
            O_V = Vector2::polar(speed, _begin_angle.radians());
        }
    }

    Path::Segment::Segment(Path::Position start_position, double curve, double duration) : Segment(start_position.point,
                                                                                                   start_position.course,
                                                                                                   curve,
                                                                                                   start_position.speed *
                                                                                                   duration,
                                                                                                   duration, 0, 0) {
        if (0.0000001 < std::abs(_curve)) {
            double r = 1 / _curve;
            O_V = Vector2::polar(r, (_begin_angle + M_PI * 0.5).radians());
        } else {
            double speed = _length / _duration;
            O_V = Vector2::polar(speed, _begin_angle.radians());
        }
    }


    void Path::appendSegment(Path::Segment segment) {

        double key = segment._duration;
        if (segments.empty()) {
            key += start_time;
        } else {
            key += segments.rbegin()->first;
        }

        segments.push_back({key, segment});
    }


    Path::constItr Path::segment(double t) const {
        if (t > 0) {
            constItr it;
            auto first = segments.begin();
            auto count = segments.size();
            while (count > 0) {
                it = first;
                auto step = count / 2;
                std::advance(it, step);
                if (it->first < t) {
                    first = ++it;
                    count -= step + 1;
                } else
                    count = step;
            }

            if (first != segments.end()) return first;
        }
        throw std::out_of_range("Time exceeds duration of path");
    }

    Path::Position Path::position(double t) const {
        auto segment_it = segment(t);
        auto time = t + segment_it->second._duration - segment_it->first;
        return segment_it->second.position(time);
    }

    void Path::cut(double t) {
        auto segment_it = segment(t);
        auto segment = segment_it->second;
        segment.cut(t + segment_it->second._duration - segment_it->first);
        segments.erase(segment_it, segments.end());
        appendSegment(segment);
    }

    std::pair<double, Path::constItr> Path::closestSegment(Vector2 point) const {
        if (segments.begin() == segments.end()) throw std::runtime_error("Path is empty");
        auto first = segments.begin();
        auto last = segments.end();

        std::pair<double, constItr> smallest = {first->second.distance(point), first};
        ++first;
        for (; first != last; ++first) {
            auto d = first->second.distance(point);
            if (d < smallest.first) {
                smallest = {d, first};
            }
        }
        return smallest;
    }


    Path::Path(const CurvedPath& curved_path, const Frame& reference_frame) : start_time(
            static_cast<double>(curved_path.start_time)) {
        for (const auto& segment : curved_path.items) {
            Vector2 localPos = reference_frame.fromWgs(segment.lat, segment.lon);
            appendSegment({localPos, degrees_to_radians(segment.begin_angle), segment.curve, segment.length,
                           segment.duration, segment.port_dev, segment.starboard_dev});
        }
    }


    Path::Position Path::endPosition() const {
        if (segments.empty())
            throw std::runtime_error("Empty Path");
        auto itr = segments.end();
        itr--;
        return itr->second.end();
    }

    std::vector<Vector2> Path::getPointsPath(const double angle_increment) const {
        std::vector<Vector2> points;
        // we'll need at least two points for every segment
        points.reserve(segments.size() * 2);
        for (const auto& segment:segments) {
            auto& s = segment.second;
            points.push_back(s._start_point);
            if (0.0000001 < std::abs(s._curve)) {
                // For arcs
                auto b_cos = std::cos(s._begin_angle.radians());
                auto b_sin = std::sin(s._begin_angle.radians());
                double r = std::abs(1 / s._curve);
                auto dangle = std::abs(s._length * s._curve);
                double sign = s._curve > 0 ? 1 : -1;
                size_t n = static_cast<size_t>(dangle / angle_increment);
                for (size_t i = 0; i < n; ++i) {
                    auto x_ = std::sin(angle_increment * i);
                    auto y_ = sign * (1 - std::cos(angle_increment * i));
                    points.push_back(s._start_point + r * Vector2(x_ * b_cos - y_ * b_sin, x_ * b_sin + y_ * b_cos));
                }
            } else {
                points.push_back(s._start_point + s.O_V * s._duration);
            }
        }
        return points;
    }
}
