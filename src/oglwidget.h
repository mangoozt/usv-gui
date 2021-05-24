#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <glm/glm.hpp>
#include <nanovg.h>
#include "usvdata/CaseData.h"

class Compass;

class Program;

class Buffer;

class GLSea;

class GLGrid;

class GLRestrictions;

struct Vessel {
    const USV::Ship *ship;
    USV::Vector2 position;
    double course; // radians
    double radius;
    glm::vec4 color{0, 0, 0, 1};
};

class OGLWidget {
public:

    struct AppearanceSettings {
        glm::vec4 sea_ambient;
        glm::vec4 sea_diffuse;
        glm::vec4 sea_specular;
        float sea_shininess;
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

    void updateTime(double t);

    void scroll([[maybe_unused]] double dx, double dy);

    void keyPress(int key);

    void mousePressEvent(double x, double y, int button, int mods);

    void mouseMoveEvent(double x, double y, bool lbutton, bool mbutton);

    void updateSunAngle(long timestamp, double lat, double lon);

    void updateAppearanceSettings(const AppearanceSettings &settings);

    [[nodiscard]] const AppearanceSettings &getAppearanceSettings() const;

protected:
    unsigned int vao{};
    std::unique_ptr<Program> m_program{nullptr};
    std::unique_ptr<Buffer> m_ship_vbo{};
    std::unique_ptr<Buffer> m_circle_vbo{};
    std::unique_ptr<Buffer> m_vessels{};
    std::unique_ptr<Buffer> m_paths{};
    unsigned int ubo_matrices{};
    unsigned int ubo_light{};
    AppearanceSettings appearance_settings;

    struct pathVBOMeta {
        size_t ptr;
        const USV::Path *path;
        size_t points_count;
        glm::vec4 color;

        pathVBOMeta(size_t ptr, const USV::Path *path, size_t points_count, glm::vec4 color) : ptr(ptr),
                                                                                               path(path),
                                                                                               points_count(points_count),
                                                                                               color(color) {};
    };

    std::vector<pathVBOMeta> m_paths_meta;
    std::vector<Vessel> vessels;
    glm::ivec2 mouse_press_point{};

    int m_myMatrixLoc{};
    int m_lightPosLoc{};
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
