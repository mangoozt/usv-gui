#ifndef USV_GUI_SKYBOX_H
#define USV_GUI_SKYBOX_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

class Skybox {

public:
    Skybox();

    ~Skybox();

    void render(QMatrix4x4& view_matrix, QVector3D eyePos);

private:

    QOpenGLShaderProgram* m_program;
    QOpenGLBuffer* vbo;
    int m_viewMatrixLoc;
    int m_viewLoc;
    int m_timeLoc;
};


#endif //USV_GUI_SKYBOX_H
