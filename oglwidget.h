#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QWheelEvent>
#include "usvdata/CaseData.h"

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class OGLWidget : public QOpenGLWidget
{
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    QVector3D screenToWorld(QPoint pos);
    void loadData(USV::CaseData &case_data);
    void updatePositions(const std::vector<USV::Vessel>& vessels);
    void wheelEvent ( QWheelEvent * event );
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
signals:
    void keyCaught(QKeyEvent *e);
protected:
    QOpenGLTexture *m_texture;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *m_vbo;
    QOpenGLBuffer *m_vessels;
    QOpenGLBuffer *m_paths;
    struct pathVBOMeta{
        size_t ptr;
        size_t points_count;
        QVector4D color;
        pathVBOMeta(size_t ptr, size_t points_count,QVector4D color):ptr(ptr),points_count(points_count),color(color){};
    };
    std::vector<pathVBOMeta> m_paths_meta;

    QOpenGLVertexArrayObject *m_vao;
    int m_projMatrixLoc;
    int m_camMatrixLoc;
    int m_worldMatrixLoc;
    int m_myMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_world;
    QMatrix4x4 m_m;
    QVector3D m_eye;
    QVector3D m_target;
    bool m_uniformsDirty;
public:
    USV::CaseData case_data{};
};

#endif // OGLWIDGET_H
