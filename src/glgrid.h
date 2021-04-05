#ifndef GLGRID_H
#define GLGRID_H

#include <glm/glm.hpp>
#include <memory>

class Program;
class Buffer;

class GLGrid {
public:
    GLGrid();

    void render();

    static const char* xyGridShaderSource;
private:
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
    int m_colorLoc;
};

#endif // GLGRID_H
