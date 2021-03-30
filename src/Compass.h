#ifndef USV_GUI_COMPASS_H
#define USV_GUI_COMPASS_H

#include <nanovg.h>

class Compass {
    constexpr static const float size{80};
    constexpr static const float radius_ratio{0.4};
    float m_x{};
    float m_y{};
    bool m_hover{false};
public:
    [[nodiscard]] bool isHover() const;

    [[nodiscard]] bool isMouseOver(float x, float y) const;

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
    void draw(NVGcontext* ctx, float angle);

    /**
     * Set position of upper-left corner of compass bounding box
     * @param x [px]
     * @param y [px]
     */
    void set_position(float x, float y);

    /**
     * Get size of compass
     * @return [px]
     */
    constexpr static float getSize() {
        return size;
    }
};


#endif //USV_GUI_COMPASS_H
