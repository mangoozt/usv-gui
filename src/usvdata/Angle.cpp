#include "Angle.h"
#include <stdexcept>

namespace USV {
    Angle::Angle(const double& angle) : value(wrap_angle(angle)) {
        if (std::isinf(angle) || std::isnan(angle))
            throw std::runtime_error("Non-normal angle value");
    }

    double wrap_angle(double angle) {
        if (angle > M_PI || angle < -M_PI) {
            if (angle < 0.0) {
                angle = fmod(angle - M_PI, M_2PI) + M_PI;
            } else {
                angle = fmod(angle + M_PI, M_2PI) - M_PI;
            }
        }
        return angle;
    }

    double radians_to_degrees(const double& radians) {
        static const double rads_to_degs = 180.0 / M_PI;
        return radians * rads_to_degs;
    }

    double degrees_to_radians(const double& degrees) {
        static const double degs_to_rads = M_PI / 180.0;
        return degrees * degs_to_rads;
    }

    bool Angle::between(Angle& min, Angle& max) const {
        auto m = (max.value - min.value) < 0.0f ? max.value - min.value + M_2PI : max.value - min.value;
        auto mid = (this->value - min.value) < 0.0f ? this->value - min.value + M_2PI : this->value - min.value;
        return (mid < m);
    }
} // namespace USV