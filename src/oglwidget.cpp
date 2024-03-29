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
#include <sstream>

#define FOV 90.0f
#define PATH_POINT_MARK_N 5

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "layout(location = 1) in vec4 position;\n"
        "layout(location = 2) in float w;\n"
        "layout(location = 3) in vec3 col;\n"
        "layout(location = 4) in float scale;\n"
        "out vec3 color;\n"
        "uniform mat4 myMatrix;\n"
        "void main() {\n"
        "   mat4 rot = mat4(cos(w),sin(w),0,0, -sin(w),cos(w),0,0, 0,0,1,0, 0,0,0,1);\n"
        "   mat4 translate = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, position.x,position.y,position.z,1);\n"
        "   mat4 m_scale = mat4(scale,0,0,0, 0,scale,0,0, 0,0,scale,0, 0,0,0,1);\n"
        "   color = col;\n"
        "   gl_Position = myMatrix *(translate*rot*m_scale*vertex);\n"
        "}\n";

static const char* fragmentShaderSource =
        "#version 330\n"
        "in highp vec3 vert;\n"
        "in highp vec3 color;\n"
        "out highp vec4 fragColor;\n"
        "uniform highp vec3 lightPos;\n"
        "void main() {\n"
        "   fragColor = vec4(color,1.0);\n"
        "}\n";


OGLWidget::OGLWidget() : m_uniformsDirty(true), compass(new Compass()), vessels(new GLVessels()) {}

void OGLWidget::initializeGL() {

    if (!m_program) {
        m_program = std::make_unique<Program>();
    }

    m_program->addVertexShader(vertexShaderSource);

    m_program->addFragmentShader(fragmentShaderSource);

    m_program->link();

    // Matrices Uniform buffer
    glGenBuffers(1, &ubo_matrices);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * 16 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, USV_GUI_MATRICES_BINDING, ubo_matrices, 0, 2 * 16 * sizeof(float));
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

    m_myMatrixLoc = m_program->uniformLocation("myMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    m_program->bind();

    m_paths = std::make_unique<Buffer>();
    m_paths->create();

    grid = std::make_unique<GLGrid>();
    sea = std::make_unique<GLSea>();
    restrictions = std::make_unique<GLRestrictions>();
//    skybox = new Skybox();
    updateAppearanceSettings({
        {0, 0.0388058, 0.123756, 1}, // sea ambient
        {0.281572, 0.442459, 0.850248, 1}, // sea diffuse
                                     {0, 0, 0, 1}, // sea specular
                                     256, // sea shininess
                                     {
                                             glm::vec4(0.724168, 0.63479, 0, 1),         //  TargetManeuver
                                             glm::vec4(0.7f, 0.7f, 0.5f, 1),//  TargetRealManeuver
                                             glm::vec4(0.769072533, 0.0, 1, 1),//  ShipManeuver
                                             glm::vec4(0, 0.600625, 1, 1)//  Route
                                     },
                                     {{
                                              {0, 1, 0, 1}, // TargetNotDangerous
                                              {1.0f, 0.6, 0.2, 1.0f}, // TargetPotentiallyDangerous
                                              {0.8, 0, 0, 1}, // TargetDangerous
                                              {0, 0.426016808, 1, 1}, // TargetUndefined
                                              {0, 0.333551884, 1, 1}, // TargetOnRealManeuver
                                              {0.7, 0.7, 0.7, 1}, // TargetInitPosition
                                              {0.8f, 0.8f, 0.8f, 1.0f}, // ShipOnRoute
                                              {0.66566503, 0.0, 0.738230228, 1.0f}, // ShipOnManeuver
                                              {0.4, 0.4, 1.0, 1.0f} // ShipInitPosition
                                      }}
                             });
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
    m_program->bind();

    if (m_uniformsDirty) {
        updateUniforms();
    }

    glDisableVertexAttribArray(1);
    glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 0.0f);
    glDisableVertexAttribArray(2);
    glVertexAttrib1f(2, 0.0f);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glVertexAttrib1f(4, 1.0f);
    m_program->release();
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
        m_program->bind();
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Draw paths
        m_paths->bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 0.0f);
        m_paths->release();

        for (const auto &path_meta:m_paths_meta) {
            const auto& color = appearance_settings.path_colors[static_cast<size_t>(path_meta.type)];
            glVertexAttrib3f(3, color.x, color.y, color.z);
            glDrawArrays(GL_LINE_STRIP, (GLint) path_meta.ptr, (GLsizei) path_meta.points_count);
        }

        // Paths start points
        m_paths->bind();
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glVertexAttrib1f(4, 0.05f); //scale
        for (const auto &path_meta:m_paths_meta) {
            const auto& color = appearance_settings.path_colors[static_cast<size_t>(path_meta.type)];
            glVertexAttrib3f(3, color.x, color.y, color.z);
            for (const auto& segment: path_meta.path->getSegments()) {
                const auto start_point = segment.second.getStartPoint();
                glVertexAttrib2f(1, (GLfloat) start_point.x(), (GLfloat) start_point.y());
                glVertexAttrib1f(2, (GLfloat) segment.second.getBeginAngle().radians());
                glDrawArrays(GL_LINE_LOOP, 0, PATH_POINT_MARK_N);
            }
        }
        m_paths->release();
        glVertexAttribDivisor(1, 0);
        glVertexAttribDivisor(2, 0);
        glVertexAttribDivisor(3, 0);
        glVertexAttribDivisor(4, 0);

        m_program->release();

        vessels->render(m_eye);

//      Draw ship captions
        float font_size{16};
        nvgFontSize(ctx, font_size);
        nvgFontFace(ctx, "sans");
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFillColor(ctx, {1, 1.0, 1, 1});
        for (const auto& vessel : vessels->getVessels()) {
            auto coord = WorldToscreen({vessel.position.x(), vessel.position.y()});
            nvgText(ctx, coord.x, coord.y, vessel.ship->name.c_str(), nullptr);
        }

        // Draw segments courses
        for (const auto &path_meta:m_paths_meta) {
            const auto& color = appearance_settings.path_colors[static_cast<size_t>(path_meta.type)];
            if(path_meta.type == USV::PathType::WastedManeuver){
                continue;
            }
            glVertexAttrib3f(3, color.x, color.y, color.z);
            for (const auto& segment: path_meta.path->getSegments()) {
                const auto start_point = segment.second.getStartPoint();
                auto c = WorldToscreen({start_point.x(), start_point.y()});
                nvgTranslate(ctx, c.x, c.y);
                nvgRotate(ctx, (GLfloat) (-segment.second.getBeginAngle().radians() + rotation + M_PI_2));

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
            auto _vessels = vessels->getVessels();
            for (size_t i = 0; i < _vessels.size(); ++i) {
                const auto& a = _vessels[i].position;
                if(_vessels[i].type==Vessel::Type::ShipOnWastedManeuver){
                    continue;
                }
                for (size_t j = i + 1; j < _vessels.size(); ++j) {
                    if(_vessels[j].type==Vessel::Type::ShipOnWastedManeuver){
                        continue;
                    }
                    const auto& b = _vessels[j].position;
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

    std::vector<GLfloat> paths;
    paths.push_back(static_cast<float>(0));paths.push_back(static_cast<float>(-1));
    paths.push_back(static_cast<float>(0));paths.push_back(static_cast<float>(-0.3));
    paths.push_back(static_cast<float>(0.3));paths.push_back(static_cast<float>(0.0));
    paths.push_back(static_cast<float>(0));paths.push_back(static_cast<float>(0.3));
    paths.push_back(static_cast<float>(0));paths.push_back(static_cast<float>(1));
    m_paths_meta.clear();
    for (const auto &pe : caseData.paths) {
        auto path_points = pe.path.getPointsPath();
        size_t ptr = paths.size() / 2;
        for (const auto &v: path_points) {
            paths.push_back(static_cast<float>(v.x()));
            paths.push_back(static_cast<float>(v.y()));
        }
        m_paths_meta.emplace_back(ptr, &pe.path, path_points.size(), pe.pathType);
    }

    m_paths->bind();
    m_paths->allocate(paths.data(), (int) (sizeof(GLfloat) * paths.size()));
    m_paths->release();

    restrictions->load_restrictions(caseData.restrictions);
}


void OGLWidget::updatePositions(const std::vector<Vessel>& new_vessels) {
    vessels->setVessels(new_vessels);
    vessels->updatePositions();
}

void OGLWidget::updatePositions() {
    vessels->updatePositions();
}

void OGLWidget::updateTime(double t) {
    time = t;
}


void OGLWidget::mousePressEvent(double x, double y, int /*button*/, int /*mods*/) {
    auto world_position = screenToWorld({x, y});
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
    glm::mat4 camera;

    const constexpr auto phi_rad = static_cast<float>(FOV / 180.0 * M_PI);

    const auto b = m_eye.z + 2;
    const auto a = std::max(b - 4, 0.01f);

    const auto aspect = W / H;
    if (aspect > 0.0001f)
        m_proj = glm::perspective(phi_rad, aspect,
                                  static_cast<float>(a),
                                  static_cast<float>(b));

    auto target = glm::vec3(m_eye.x, m_eye.y, 0);
    camera = glm::lookAt(m_eye, target, glm::vec3(std::cos(rotation), std::sin(rotation), 0));

    // Update matrices UBO
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &m_proj);
    const auto m_view = camera;
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), &m_view);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_m = m_proj * m_view;
    m_program->setUniformValue(m_myMatrixLoc, m_m);
    m_program->setUniformValue(m_lightPosLoc, glm::vec3(0, 0, 70));

    m_uniformsDirty = false;
}

void OGLWidget::updateAppearanceSettings(const OGLWidget::AppearanceSettings &settings) {
    appearance_settings = settings;
    auto a = appearance_settings.path_colors[static_cast<int>(USV::PathType::Route)];
    Material sea_material{
            appearance_settings.sea_ambient,
            appearance_settings.sea_diffuse,
            appearance_settings.sea_specular,
            appearance_settings.sea_shininess,
    };
    sea->set_material(sea_material);
    vessels->setAppearanceSettings(appearance_settings.vessels_colors);
}

const OGLWidget::AppearanceSettings &OGLWidget::getAppearanceSettings() const {
    return appearance_settings;
}

OGLWidget::~OGLWidget() = default;
