#ifndef USV_ANGLE_H
#define USV_ANGLE_H
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_2PI
    #define M_2PI       6.28318530717958647692528676655900576
#endif

namespace USV {


    double radians_to_degrees(const double& radians);

    double degrees_to_radians(const double& degrees);

    double wrap_angle(double angle);

    class Angle {
    public:
        Angle(const double& angle = 0.0);

        Angle& operator=(const double& angle);

        Angle operator+(const double& angle) const;

        Angle operator+(const Angle& angle) const;

        void operator+=(const double& angle);

        void operator+=(const Angle& angle);

        Angle operator-(const double& angle) const;

        Angle operator-(const Angle& angle) const;

        Angle operator-() const;

        void operator-=(const double& angle);

        void operator-=(const Angle& angle);

        Angle operator*(const double& scalar) const;

        void operator*=(const double& scalar);

        explicit operator const double&() const { return value; }

        [[nodiscard]] double degrees() const;

        [[nodiscard]] double radians() const;

        bool between(Angle& min, Angle& max) const;

        static Angle Degrees(const double& angle);

        static Angle Radians(const double& angle);

    private:
        double value;
    };


    inline double Angle::degrees() const {
        return radians_to_degrees(value);
    }

    inline double Angle::radians() const {
        return value;
    }


    inline Angle& Angle::operator=(const double& angle) {
        value = angle;
        value = wrap_angle(value);
        return *this;
    }


    inline Angle Angle::operator+(const double& angle) const {
        return {wrap_angle(value + angle)};
    }

    inline Angle Angle::operator+(const Angle& angle
    ) const {
        return {wrap_angle(value + angle.value)};
    }

    inline void Angle::operator+=(const double& angle) {
        value += angle;
        value = wrap_angle(value);
    }

    inline void Angle::operator+=(const Angle& angle) {
        value += angle.value;
        value = wrap_angle(value);
    }

    inline Angle Angle::operator-(const double& angle) const {
        return {wrap_angle(value - angle)};
    }

    inline Angle Angle::operator-(const Angle& angle) const {
        return {wrap_angle(value - angle.value)};
    }

    inline Angle Angle::operator-() const {
        return {-value};
    }

    inline void Angle::operator-=(const double& angle) {
        value -= angle;
        value = wrap_angle(value);
    }


    inline void Angle::operator-=(const Angle& angle) {
        value -= angle.
                value;
        value = wrap_angle(value);
    }

    inline Angle Angle::operator*(const double& scalar) const {
        return {wrap_angle(scalar * value)};
    }

    inline void Angle::operator*=(const double& scalar) {
        value *= scalar;
        value = wrap_angle(value);
    }

    inline bool operator==(const Angle& a, const Angle& b) {
        return fmod(a.radians() + M_2PI , M_2PI ) == fmod(b.radians() + M_2PI, M_2PI);
    }

    inline bool operator!=(const Angle& a, const Angle& b) {
        return !(a == b);
    }

    inline bool operator<(const Angle& a, const Angle& b) {
        return a.radians() < b.radians();
    }

    inline bool operator<=(const Angle& a, const Angle& b) {
        return (a < b) || (a == b);
    }

    inline bool operator>(const Angle& a, const Angle& b) {
        return !(a <= b);
    }

    inline bool operator>=(const Angle& a, const Angle& b) {
        return !(a < b);
    }

    inline Angle Angle::Degrees(const double& angle) {
        return {degrees_to_radians(angle)};
    }

    inline Angle Angle::Radians(const double& angle) {
        return {angle};
    }
} //namespace USV

#endif //USV_ANGLE_H
