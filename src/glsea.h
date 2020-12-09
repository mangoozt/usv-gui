#ifndef GLSEA_H
#define GLSEA_H
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

class GLSea
{
public:
    GLSea(QImage& texture, QImage& normal, QImage &specular);
    ~GLSea();
    void render(QMatrix4x4 &view_matrix, QVector3D eyePos, double time=0);
private:
    void prepare_grid();
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *vbo;
    QOpenGLBuffer *ibo;
    QOpenGLTexture tex;
    QOpenGLTexture normal_tex;
    QOpenGLTexture spec_tex;
    int m_viewLoc;
    int m_timeLoc;
    const GLuint gridsize{1000};
    const float size{100.0f};
};

#endif // GLSEA_H
