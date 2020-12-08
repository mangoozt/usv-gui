#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QWheelEvent>
#include "usvdata/CaseData.h"
#include "text.h"
#include "glgrid.h"
#include "glsea.h"
#include "glrestrictions.h"

class QOpenGLTexture;

class QOpenGLShaderProgram;

class QOpenGLBuffer;

class QOpenGLVertexArrayObject;

class OGLWidget : public QOpenGLWidget {
public:
    explicit OGLWidget(QWidget* parent = nullptr);

    ~OGLWidget() override;

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    QVector3D screenToWorld(QPoint pos);

    void loadData(USV::CaseData& case_data);

    void updatePositions(const std::vector<USV::Vessel>& vessels);

    void updateTime(double t);

    void wheelEvent(QWheelEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

signals:

    void keyCaught(QKeyEvent* e);

protected:
    QOpenGLTexture* m_texture;
    QOpenGLShaderProgram* m_program;
    QOpenGLBuffer* m_ship_vbo;
    QOpenGLBuffer* m_circle_vbo;
    QOpenGLBuffer* m_vessels{};
    QOpenGLBuffer* m_paths{};

    struct pathVBOMeta {
        size_t ptr;
        size_t points_count;
        QVector4D color;

        pathVBOMeta(size_t ptr, size_t points_count, QVector4D color) : ptr(ptr), points_count(points_count), color(
                color) {};
    };

    std::vector<pathVBOMeta> m_paths_meta;
    QPoint mouse_press_point;

    QOpenGLVertexArrayObject* m_vao;
    int m_myMatrixLoc{};
    int m_lightPosLoc{};
    QMatrix4x4 m_proj;
    QMatrix4x4 m_world;
    QMatrix4x4 m_m;
    QVector3D m_eye;
    double rotation;
    bool m_uniformsDirty;
    Text* text{};
    GLGrid* grid{};
    GLSea* sea{};
    GLRestrictions* restrictions{};
    double time{0.0f};
    std::unique_ptr<USV::CaseData> case_data_;
public:
    [[nodiscard]] const USV::CaseData* case_data() const {
        return case_data_.get();
    }
};

#endif // OGLWIDGET_H
