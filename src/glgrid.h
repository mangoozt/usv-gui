#ifndef GLGRID_H
#define GLGRID_H

#include <glm/glm.hpp>
#include "Program.h"
#include "Buffer.h"
#include <memory>

class GLGrid {
public:
    GLGrid();

    void render(const glm::mat4& view_matrix);

    static const char* xyGridShaderSource;
private:
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
    int m_viewMatrixLoc;
    int m_colorLoc;
};

#endif // GLGRID_H
