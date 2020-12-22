#ifndef USV_GUI_SKYBOX_H
#define USV_GUI_SKYBOX_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class Skybox {

public:
    Skybox();

    ~Skybox();

    void render();

private:

    QOpenGLShaderProgram* m_program;
    QOpenGLBuffer* vbo;
};


#endif //USV_GUI_SKYBOX_H
