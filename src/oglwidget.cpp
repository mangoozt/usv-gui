#include "oglwidget.h"
#include <cmath>
#include "Program.h"
#include <glm/ext.hpp>
#include <nanovg.h>
#include <GLFW/glfw3.h>
#include "Defines.h"
#include "Compass.h"
#include "Buffer.h"
#include "glsea.h"
#include "glgrid.h"
#include "glrestrictions.h"
#include "vessel_list.h"
#include "path_appearance.h"
#include "provider.h"
#include <sstream>

#define FOV 90.0f

OGLWidget::OGLWidget() : m_uniformsDirty(true), compass(new Compass()) {}

void OGLWidget::initializeGL() {
    // Matrices Uniform buffer
    glGenBuffers(1, &ubo_matrices);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferData(GL_UNIFORM_BUFFER, 3 * 16 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, USV_GUI_MATRICES_BINDING, ubo_matrices, 0, 3 * 16 * sizeof(float));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    struct LightSource {
        glm::vec4 position = glm::vec4(-100.0f, 100.0f, 10.0f, 0);
        glm::vec4 ambient = glm::vec4(0.7f, 0.7f, 0.7f, 0);
        glm::vec4 diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 0);
        glm::vec4 specular = glm::vec4(1, 1, 1, 0);
    } light;

    // Light Uniform buffer
    glGenBuffers(1, &ubo_light);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSource), &light, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, USV_GUI_LIGHTS_BINDING, ubo_light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    grid = std::make_unique<GLGrid>();
    sea = std::make_unique<GLSea>();
    restrictions = std::make_unique<GLRestrictions>();
    paths = std::make_unique<GLPaths>();
    vessels = std::make_unique<GLVessels>();
//    skybox = new Skybox();
}

void OGLWidget::resizeGL(int w, int h) {
    width = w;
    height = h;
    constexpr static const float compass_margins{10};
    compass->set_position(static_cast<float>(width) - Compass::getSize() - compass_margins, compass_margins);
    m_uniformsDirty = true;
}

void OGLWidget::paintGL(NVGcontext *ctx) {
    int m_viewport_backup[4], m_scissor_backup[4];
    bool m_depth_test_backup;
    bool m_depth_write_backup;
    bool m_scissor_test_backup;
    bool m_cull_face_backup;
    bool m_blend_backup;

    glGetIntegerv(GL_VIEWPORT, m_viewport_backup);
    glGetIntegerv(GL_SCISSOR_BOX, m_scissor_backup);
    GLboolean depth_write;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_write);
    m_depth_write_backup = depth_write;

    m_depth_test_backup = glIsEnabled(GL_DEPTH_TEST);
    m_scissor_test_backup = glIsEnabled(GL_SCISSOR_TEST);
    m_cull_face_backup = glIsEnabled(GL_CULL_FACE);
    m_blend_backup = glIsEnabled(GL_BLEND);

    glBindVertexArray(vao);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if (m_uniformsDirty) {
        updateUniforms();
    }
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    restrictions->render(m_eye, GLRestrictions::GeometryTypes::Isle);
    glStencilMask(0x00);
    sea->render(m_eye, time);
    //Draw plane
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    grid->render();
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

    restrictions->render(m_eye, GLRestrictions::GeometryTypes::All ^ GLRestrictions::GeometryTypes::Isle);
    if (case_data_ != nullptr) {
        paths->render();
        vessels->render(m_eye);

//      Draw ship captions
        float font_size{16};
        nvgFontSize(ctx, font_size);
        nvgFontFace(ctx, "sans");
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFillColor(ctx, {1, 1.0, 1, 1});

        const std::vector<Vessel> &vessel_list = Provider<VesselListNotifier>::of().value;

        for (const auto& vessel : vessel_list) {
            auto coord = WorldToscreen({vessel.position.x(), vessel.position.y()});
            nvgText(ctx, coord.x, coord.y, vessel.ship->name.c_str(), nullptr);
        }

        const PathAppearance path_appearance = Provider<PathAppearanceNotifier>::of().value;

        // Draw segments courses
        for (const auto &path: pathsInfo) {
            USV::PathType type = path.getType();
            const auto &color = path_appearance.getColor(path);
            if (type == USV::PathType::WastedManeuver) {
              continue;
            }

            if (!path_appearance.isVisible(path)) {
              continue;
            }

            glVertexAttrib3f(3, color.x, color.y, color.z);
            for (const auto& segment: path.getSegments()) {
                const auto& start_point = segment.second.getStartPoint();
                const auto c = WorldToscreen({start_point.x(), start_point.y()});
                nvgTranslate(ctx, c.x, c.y);
                nvgRotate(ctx, static_cast<GLfloat>(-segment.second.getBeginAngle().radians() + rotation + M_PI_2));

                std::stringstream tmp;
                tmp << std::setw(5) << fmod(450 - segment.second.getBeginAngle().degrees(), 360) << "°";

                nvgText(ctx, 0.0, 0.0, tmp.str().c_str(), nullptr);
                nvgResetTransform(ctx);

//                glVertexAttrib2f(1, (GLfloat) start_point.x(), (GLfloat) start_point.y());
//                glVertexAttrib1f(2, (GLfloat) segment.second.getBeginAngle().radians());
//                glDrawArrays(GL_LINE_LOOP, 0, PATH_POINT_MARK_N);
            }
        }
        // Draw distances
        {
            nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
            nvgFontSize(ctx, 14);
            nvgStrokeWidth(ctx, 1.0f);
            nvgStrokeColor(ctx, {1.0, 1.0, 1.0, 1.0});
            const auto distance_capSq = distance_cap * distance_cap;
            for (size_t i = 0; i < vessel_list.size(); ++i) {
                const auto& a = vessel_list[i].position;
                if (vessel_list[i].type == Vessel::Type::ShipOnWastedManeuver) {
                  continue;
                }
                for (size_t j = i + 1; j < vessel_list.size(); ++j) {
                    if(vessel_list[j].type==Vessel::Type::ShipOnWastedManeuver){
                        continue;
                    }
                    const auto& b = vessel_list[j].position;
                    const auto ba = a - b;
                    const auto ba_Sq = absSq(ba);
                    if (ba_Sq > distance_capSq || ba_Sq < 1)
                        continue;

                    const auto m = (a + b) * 0.5;

                    const auto angle = static_cast<float>(fmod(atan2(ba.x(), ba.y()) - M_PI, M_PI));
                    auto c = WorldToscreen({a.x(), a.y()});

                    nvgBeginPath(ctx);
                    nvgMoveTo(ctx, c.x, c.y);
                    c = WorldToscreen({b.x(), b.y()});
                    nvgLineTo(ctx, c.x, c.y);
                    c = WorldToscreen({m.x(), m.y()});
                    nvgStroke(ctx);

                    nvgTranslate(ctx, c.x, c.y);
                    nvgRotate(ctx, angle + rotation);

                    std::stringstream tmp;
                    tmp << std::fixed << std::setprecision( 2 ) << abs(ba);

                    nvgText(ctx, 0.0, 0.0, tmp.str().c_str(), nullptr);
                    nvgResetTransform(ctx);
                }
            }
        }
    }
    compass->draw(ctx, rotation);
//    skybox->render();


    glViewport(m_viewport_backup[0], m_viewport_backup[1], m_viewport_backup[2], m_viewport_backup[3]);
    glScissor(m_scissor_backup[0], m_scissor_backup[1], m_scissor_backup[2], m_scissor_backup[3]);

    if (m_depth_test_backup)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDepthMask(m_depth_write_backup);

    if (m_scissor_test_backup)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    if (m_cull_face_backup)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    if (m_blend_backup)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

glm::vec2 OGLWidget::WorldToscreen(glm::vec2 pos) {
    auto W = static_cast<float>(width);
    auto H = static_cast<float>(height);

    auto v = glm::vec4(pos.x, pos.y, 0, 1);
    auto p = m_m * v;
    p /= p.w;
    auto x = (p.x + 1.0f) * 0.5f * W;
    auto y = (1.0f - p.y) * 0.5f * H;

    return {x, y};
}

glm::vec3 OGLWidget::screenToWorld(glm::ivec2 pos) {
    glm::mat3 minv(m_m[0].x, m_m[0].y, m_m[0].w, m_m[1].x, m_m[1].y, m_m[1].w, m_m[3].x, m_m[3].y, m_m[3].w);
    minv = glm::inverse(minv);
    glm::vec3 point_normalized = glm::vec3((float) pos.x / (float) width * 2 - 1,
                                           1 - (float) pos.y / (float) height * 2,
                                           1.0f);

    auto position = minv * point_normalized;
    position /= position.z;
    position.z = 0;
    return position;
}

void OGLWidget::loadData(std::unique_ptr<USV::CaseData> case_data) {
    case_data_ = std::move(case_data);
    const auto& caseData = *case_data_;
    vessels->setCaseData(case_data_.get());
//    enum class PathType {
//        TargetManeuver=0,
//        TargetRealManeuver,
//        ShipManeuver,
//        Route,
//        End
//    };

    pathsInfo.clear();
    for (const auto &pe : caseData.paths) {
        pathsInfo.push_back(pe.path);
    }

    paths->initVbo(pathsInfo);

    restrictions->load_restrictions(caseData.restrictions);
}

void OGLWidget::updateTime(double t) {
    time = t;
}


void OGLWidget::mousePressEvent(double x, double y, int /*button*/, int /*mods*/) {
    mouse_press_point = {x, y};
    if (compass->isHover()) {
        if (rotation != init_rotation)
            rotation = init_rotation;
        else
            m_eye = init_m_eye;
        m_uniformsDirty = true;
    }
}

void OGLWidget::mouseMoveEvent(double x, double y, bool lbutton, bool mbutton) {
    if (lbutton | mbutton) {
        auto p = screenToWorld(mouse_press_point);
        mouse_press_point = {x, y};
        auto p1 = screenToWorld(mouse_press_point);

        auto dp = p - p1;

        if (lbutton) {
            m_eye.x = glm::clamp(m_eye.x + dp.x, -200.0f, 200.0f);
            m_eye.y = glm::clamp(m_eye.y + dp.y, -200.0f, 200.0f);
        } else {
            p = p - glm::vec3(m_eye.x, m_eye.y, 0);
            dp.z = 0;
            rotation += glm::cross(p, dp).z / (p.x*p.x+p.y*p.y);
        }
        m_uniformsDirty = true;
    }
    m_uniformsDirty |= compass->setHover(compass->isMouseOver(x, y));
}

void OGLWidget::scroll(double /*dx*/, double dy) {
    auto delta = static_cast<GLfloat>(dy);
    m_eye.z = glm::clamp(m_eye.z - delta, 2.0f, 40.0f / std::tan(FOV/2));
    m_uniformsDirty = true;
}


void OGLWidget::keyPress(int key) {
    static constexpr float rotation_step = 10.0f / 360.0f * (float) M_PI;
    static constexpr float k_tr = 1.0f / 100;
    switch (key) {
        case GLFW_KEY_W:
            m_eye.y = glm::clamp(m_eye.y + m_eye.z * k_tr * std::sin(rotation), -60.0f, 60.0f);
            m_eye.x = glm::clamp(m_eye.x + m_eye.z * k_tr * std::cos(rotation), -60.0f, 60.0f);
            break;
        case GLFW_KEY_S:
            m_eye.y = glm::clamp(m_eye.y - m_eye.z * k_tr * std::sin(rotation), -60.0f, 60.0f);
            m_eye.x = glm::clamp(m_eye.x - m_eye.z * k_tr * std::cos(rotation), -60.0f, 60.0f);
            break;
        case GLFW_KEY_D:
            m_eye.x = glm::clamp(m_eye.x + m_eye.z * k_tr * std::sin(rotation), -60.0f, 60.0f);
            m_eye.y = glm::clamp(m_eye.y - m_eye.z * k_tr * std::cos(rotation), -60.0f, 60.0f);
            break;
        case GLFW_KEY_A:
            m_eye.x = glm::clamp(m_eye.x - m_eye.z * k_tr * std::sin(rotation), -60.0f, 60.0f);
            m_eye.y = glm::clamp(m_eye.y + m_eye.z * k_tr * std::cos(rotation), -60.0f, 60.0f);
            break;
        case GLFW_KEY_E:
            rotation += rotation_step;
            break;
        case GLFW_KEY_Q:
            rotation -= rotation_step;
            break;
        default:
            return;
    }
    m_uniformsDirty = true;
}

void OGLWidget::updateSunAngle(long timestamp, double lat, double /*lon*/) {
    tm tm_{};
    time_t timet = timestamp;

    tm* ptm = gmtime(&timet);

    tm_ = *ptm;

    int year_day = tm_.tm_yday;
    auto delta = 23.45 * sin(360 / 365.0 * (year_day + 284)); //degrees
    auto local_time = tm_.tm_hour + tm_.tm_min / 60.0; // hours
    auto h = (local_time - 12) * 15; // degrees

    delta = delta / 180 * M_PI; //radians
    h = h / 180 * M_PI; //radians
    lat = lat / 180 * M_PI; // radians

    auto sin_alpha = sin(lat) * sin(delta) + cos(lat) * cos(delta) * cos(h);
    auto cos_alpha = sqrt(1 - sin_alpha * sin_alpha);
    auto sin_z = cos(delta) * sin(h) / cos_alpha;
    auto cos_z = sqrt(1 - sin_z * sin_z);

    struct LightSource {
        glm::vec4 position;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
    };
    LightSource light{
        glm::vec4(-sin_z, -cos_z, sin_alpha, 0),
            glm::vec4(0.7f, 0.7f, 0.7f, 0),
            glm::vec4(0.7f, 0.7f, 0.7f, 0),
            glm::vec4(1, 1, 1, 0)
    };

    // Light Uniform buffer
    glGenBuffers(1, &ubo_light);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_light);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSource), &light, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, USV_GUI_LIGHTS_BINDING, ubo_light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OGLWidget::updateUniforms() {
    const auto W = static_cast<float>(width);
    const auto H = static_cast<float>(height);

    m_proj = glm::identity<glm::mat4>();
    glm::mat4 m_view;

    const constexpr auto phi_rad = static_cast<float>(FOV / 180.0 * M_PI);

    const auto b = m_eye.z + 2;
    const auto a = std::max(b - 4, 0.01f);

    const auto aspect = W / H;
    if (aspect > 0.0001f)
        m_proj = glm::perspective(phi_rad, aspect,
                                  static_cast<float>(a),
                                  static_cast<float>(b));

    auto target = glm::vec3(m_eye.x, m_eye.y, 0);
    m_view = glm::lookAt(m_eye, target, glm::vec3(std::cos(rotation), std::sin(rotation), 0));
    m_m = m_proj * m_view;

    // Update matrices UBO
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &m_proj);
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), &m_view);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * 16 * sizeof(float), 16 * sizeof(float), &m_m);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_uniformsDirty = false;
}

OGLWidget::~OGLWidget() = default;
