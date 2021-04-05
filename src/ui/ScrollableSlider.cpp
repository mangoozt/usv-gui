#include "ScrollableSlider.h"

ScrollableSlider::ScrollableSlider(Widget* parent) : Slider(parent) {}

bool ScrollableSlider::scroll_event(const Vector2i& /*p*/, const Vector2f& rel) {
    if (!m_enabled)
        return false;

    if (!focused()) {
        float value_delta = (rel.y() > 0) ? 1.0f : -1.0f;
        auto value = m_value + value_delta / (float) width(), old_value = m_value;
        m_value = std::min(std::max(value, m_range.first), m_range.second);
        if (m_callback && m_value != old_value)
            m_callback(m_value);
        return true;
    }
    return false;
}
