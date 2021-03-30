#include <cmath>
#include "Compass.h"

namespace {
    constexpr NVGcolor nvRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }

    constexpr NVGcolor nvRGB(unsigned char r, unsigned char g, unsigned char b) {
        return nvRGBA(r, g, b, 255);
    }
}

void Compass::draw(NVGcontext* ctx, const float rotation) {
    constexpr const auto& cs = size;
    constexpr const auto radius = radius_ratio * size;

    constexpr const NVGcolor bright_red{1.0, 0.0, 0.0, 1.0};
    constexpr const NVGcolor pale_red{1.0, 0.4, 0.4, 1.0};
    constexpr const NVGcolor bright_silver{1.0, 1.0, 1.0, 1.0};
    constexpr const NVGcolor pale_silver = nvRGB(192, 192, 192);
    constexpr const NVGcolor circle_color{0.5, 0.5, 0.5, 0.3};
    constexpr const NVGcolor circle_hover_fill_color{0.6, 0.6, 0.6, 0.2};
    constexpr const NVGcolor pole_text_color{1, 1, 1, 1};
    constexpr const NVGcolor ew_arrows_color{1, 1, 1, 1};
    constexpr const float font_size{cs * 0.175};

    nvgTranslate(ctx, m_x + .5f * cs, m_y + 0.5f * cs);
    nvgRotate(ctx, rotation - static_cast<float>(M_PI_2));

    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx, 3);
    nvgStrokeColor(ctx, circle_color);
    nvgCircle(ctx, 0, 0, cs * 0.4f);
    nvgStroke(ctx);
    if (m_hover) {
        nvgFillColor(ctx, circle_hover_fill_color);
        nvgFill(ctx);
    }

    nvgStrokeWidth(ctx, 1);
    nvgStrokeColor(ctx, ew_arrows_color);

    // East-West arrows
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, -radius, 0);
    nvgLineTo(ctx, 0, -0.125*radius);
    nvgLineTo(ctx, radius, 0);
    nvgLineTo(ctx, 0, 0.125*radius);
    nvgClosePath(ctx);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, -radius, 0);
    nvgLineTo(ctx, radius, 0);
    nvgStroke(ctx);

    nvgStrokeWidth(ctx, 2);
    // Red north arrow
    nvgBeginPath(ctx);
    nvgFillColor(ctx, bright_red);
    nvgMoveTo(ctx, 0, -radius);
    nvgLineTo(ctx, -0.125*radius, 0);
    nvgLineTo(ctx, 0.125*radius, 0);
    nvgClosePath(ctx);
    nvgFill(ctx);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgFillColor(ctx, pale_red);
    nvgMoveTo(ctx, 0, -radius);
    nvgLineTo(ctx, -0.125*radius, 0);
    nvgLineTo(ctx, 0. * cs, 0);
    nvgClosePath(ctx);
    nvgFill(ctx);

    // Silver south arrow
    nvgBeginPath(ctx);
    nvgFillColor(ctx, bright_silver);
    nvgMoveTo(ctx, 0, radius);
    nvgLineTo(ctx, -0.125*radius, 0);
    nvgLineTo(ctx, 0.125*radius, 0);
    nvgClosePath(ctx);
    nvgFill(ctx);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgFillColor(ctx, pale_silver);
    nvgMoveTo(ctx, 0, radius);
    nvgLineTo(ctx, -0.125*radius, 0);
    nvgLineTo(ctx, 0. * cs, 0);
    nvgClosePath(ctx);
    nvgFill(ctx);

    // North pole letter
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgFillColor(ctx, pole_text_color);
    nvgFontSize(ctx, font_size);
    nvgText(ctx, 0, -radius - font_size - 3, "N", nullptr);

    nvgResetTransform(ctx);
}

void Compass::set_position(float x, float y) {
    m_x = x;
    m_y = y;
}

bool Compass::isHover() const {
    return m_hover;
}

bool Compass::setHover(bool hover) {
    auto changed = (m_hover != hover);
    m_hover = hover;
    return changed;
}

bool Compass::isMouseOver(float x, float y) const {
    x -= m_x + size * 0.5;
    y -= m_y + size * 0.5;
    return (x * x + y * y) < radius_ratio * radius_ratio * size * size;
}
