#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
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
    void loadData(USV::CaseData &case_data);
protected:

    QOpenGLTexture *m_texture;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *m_vbo;
    QOpenGLBuffer *m_vessels;
    QOpenGLVertexArrayObject *m_vao;
    int m_projMatrixLoc;
    int m_camMatrixLoc;
    int m_worldMatrixLoc;
    int m_myMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_world;
    QVector3D m_eye;
    QVector3D m_target;
    bool m_uniformsDirty;
    USV::CaseData case_data{};
};

#endif // OGLWIDGET_H
