#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <glm/glm.hpp>
#include <nanovg.h>
#include "usvdata/CaseData.h"
#include "glgrid.h"
#include "glsea.h"
#include "glrestrictions.h"
#include "skybox.h"
#include "Program.h"
#include "Buffer.h"

class OGLWidget{
public:
    OGLWidget();

    void initializeGL() ;

    void resizeGL(int w, int h) ;

    void paintGL(NVGcontext *ctx) ;

    glm::vec3 screenToWorld(glm::ivec2 pos);

    glm::vec2 WorldToscreen(glm::vec2 pos);

    void loadData(USV::CaseData& case_data);

    void updatePositions(const std::vector<USV::Vessel>& vessels);

    void updateTime(double t);

    void scroll([[maybe_unused]] double dx, double dy);

    void keyPress(int key);

    void mousePressEvent(double x, double y, int button, int action, int mods);

    void mouseMoveEvent(double x, double y, bool lbutton, bool mbutton);

    void updateSunAngle(long timestamp, double lat, double lon);

protected:
    GLuint vao{};
    std::unique_ptr<Program> m_program{nullptr};
    std::unique_ptr<Buffer> m_ship_vbo{};
    std::unique_ptr<Buffer> m_circle_vbo{};
    std::unique_ptr<Buffer> m_vessels{};
    std::unique_ptr<Buffer> m_paths{};
    GLuint ubo_matrices{};
    GLuint ubo_light{};

    struct pathVBOMeta {
        size_t ptr;
        size_t points_count;
        glm::vec4 color;

        pathVBOMeta(size_t ptr, size_t points_count, glm::vec4 color) : ptr(ptr), points_count(points_count), color(
                color) {};
    };

    std::vector<pathVBOMeta> m_paths_meta;
    glm::ivec2 mouse_press_point{};

//    QOpenGLVertexArrayObject* m_vao{};
    int m_myMatrixLoc{};
    int m_lightPosLoc{};
    glm::mat4 m_proj{};
    glm::mat4 m_m{};
    glm::vec3 m_eye;
    float rotation{static_cast<float>(M_PI * 0.5)};
    bool m_uniformsDirty;
    GLGrid* grid{};
    GLSea* sea{};
    GLRestrictions* restrictions{};
//    Skybox* skybox{};
    double time{0.0f};
    double distance_cap{12.0};
    std::unique_ptr<USV::CaseData> case_data_;
    unsigned int width{};
    unsigned int height{};
public:
    [[nodiscard]] const USV::CaseData* case_data() const {
        return case_data_.get();
    }

    [[nodiscard]] inline bool uniforms_dirty() const { return m_uniformsDirty; }
};

#endif // OGLWIDGET_H
