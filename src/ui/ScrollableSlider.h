#ifndef USV_GUI_SCROLLABLESLIDER_H
#define USV_GUI_SCROLLABLESLIDER_H

#include <nanogui/slider.h>

using namespace nanogui;

class ScrollableSlider : public Slider {
public:
    explicit ScrollableSlider(Widget* parent);

    bool scroll_event(const Vector2i& p, const Vector2f& rel) override;
};


#endif //USV_GUI_SCROLLABLESLIDER_H
