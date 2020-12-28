#ifndef USV_GUI_SKYBOX_H
#define USV_GUI_SKYBOX_H

#include <glm/glm.hpp>
#include "Program.h"
#include "Buffer.h"
#include <memory>

class Skybox {

public:
    Skybox();

    ~Skybox();

    void render();

private:
    GLint vertexLocation;
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
};


#endif //USV_GUI_SKYBOX_H
