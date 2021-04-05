#ifndef USV_GUI_COMPASS_H
#define USV_GUI_COMPASS_H

#include <nanovg.h>

class Compass {
    constexpr static const double size{80};
    constexpr static const double radius_ratio{0.4};
    double m_x{};
    double m_y{};
    bool m_hover{false};
public:
    [[nodiscard]] bool isHover() const;

    [[nodiscard]] bool isMouseOver(double x, double y) const;

    /**
     * Set hover state
     * @param hover New state value
     * @return Has value changed
     */
    bool setHover(bool hover);

public:
    /**
     * Draw a compass
     * @param ctx Pointer to NVG context
     * @param angle Compass pointing angle [rad]
     */
    void draw(NVGcontext* ctx, double angle) const;

    /**
     * Set position of upper-left corner of compass bounding box
     * @param x [px]
     * @param y [px]
     */
    void set_position(double x, double y);

    /**
     * Get size of compass
     * @return [px]
     */
    constexpr static double getSize() {
        return size;
    }
};


#endif //USV_GUI_COMPASS_H
