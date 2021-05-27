#ifndef GLSEA_H
#define GLSEA_H
#include <glm/glm.hpp>
#include "Defines.h"
#include <memory>

class Program;
class Buffer;

class GLSea
{
public:
    GLSea();
    void render(glm::vec3& eyePos, double time=0);
    void set_material(const Material& new_material);
private:
    unsigned int vertexLocation{};
    void prepare_grid();
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
    std::unique_ptr<Buffer> ibo;
    int m_viewLoc;
    int m_timeLoc;
    const unsigned int gridsize{100};
    const float size{2.0f};
};

#endif // GLSEA_H
