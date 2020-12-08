#ifndef GLGRID_H
#define GLGRID_H
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GLGrid
{
public:
    GLGrid();
    ~GLGrid();
    void render(QMatrix4x4 &view_matrix);
    static const char *xyGridShaderSource;
private:
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer *vbo;
    int m_viewMatrixLoc;
    int m_colorLoc;
};

#endif // GLGRID_H
