#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include "usvdata/CaseData.h"
#include "glvessels.h"
#include "glpaths.h"
#include <glm/glm.hpp>
#include <nanovg.h>

class Compass;

class Program;

class Buffer;

class GLSea;

class GLGrid;

class GLRestrictions;

class OGLWidget {
public:

    struct AppearanceSettings {
        glm::vec4 sea_ambient;
        glm::vec4 sea_diffuse;
        glm::vec4 sea_specular;
        float sea_shininess;
        GLPaths::AppearenceSettings path_colors{};
        GLVessels::AppearanceSettings vessels_colors{};
    };

    OGLWidget();

    virtual ~OGLWidget();

    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL(NVGcontext *ctx);

    glm::vec3 screenToWorld(glm::ivec2 pos);

    glm::vec2 WorldToscreen(glm::vec2 pos);

    void loadData(std::unique_ptr<USV::CaseData> case_data);

    void updatePositions(const std::vector<Vessel> &vessels);

    void updatePositions();

    void updateTime(double t);

    void scroll([[maybe_unused]] double dx, double dy);

    void keyPress(int key);

    void mousePressEvent(double x, double y, int button, int mods);

    void mouseMoveEvent(double x, double y, bool lbutton, bool mbutton);

    void updateSunAngle(long timestamp, double lat, double lon);

    void updateAppearanceSettings(const AppearanceSettings &settings);

    void showManeuvers(bool should_show);

    [[nodiscard]] const AppearanceSettings &getAppearanceSettings() const;

protected:
    unsigned int vao{};
    unsigned int ubo_matrices{};
    unsigned int ubo_light{};
    AppearanceSettings appearance_settings;

    std::vector<USV::Path> pathsInfo;
    bool show_maneuvers{true};

    glm::ivec2 mouse_press_point{};

    glm::mat4 m_proj{};
    glm::mat4 m_m{};
    constexpr static const glm::vec3 init_m_eye{0, 0, 20};
    glm::vec3 m_eye{init_m_eye};
    static constexpr const float init_rotation{static_cast<float>(M_PI * 0.5)};
    float rotation{init_rotation};
    bool m_uniformsDirty;
    std::unique_ptr<GLGrid> grid{};
    std::unique_ptr<GLSea> sea{};
    std::unique_ptr<GLRestrictions> restrictions{};
    std::unique_ptr<Compass> compass;
    std::unique_ptr<GLVessels> vessels;
    std::unique_ptr<GLPaths> paths;

    double time{0.0f};
    double distance_cap{12.0};
    std::unique_ptr<USV::CaseData> case_data_;
    unsigned int width{};
    unsigned int height{};

    void updateUniforms();

public:
    [[nodiscard]] const USV::CaseData *case_data() const {
        return case_data_.get();
    }

    [[nodiscard]] inline bool uniforms_dirty() const { return m_uniformsDirty; }
};

#endif // OGLWIDGET_H
