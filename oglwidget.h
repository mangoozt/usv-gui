#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class OGLWidget : public QOpenGLWidget
{

    Q_OBJECT
    Q_PROPERTY(float z READ z WRITE setZ)
    Q_PROPERTY(float r READ r WRITE setR)
    Q_PROPERTY(float r2 READ r2 WRITE setR2)
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();

    void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        float z() const { return m_eye.z(); }
        void setZ(float v);

        float r() const { return m_r; }
        void setR(float v);
        float r2() const { return m_r2; }
        void setR2(float v);

protected:

    QOpenGLTexture *m_texture;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *m_vbo;
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
    float m_r;
    float m_r2;
};

#endif // OGLWIDGET_H
