#ifndef GLSEA_H
#define GLSEA_H
#include <glm/glm.hpp>
#include "Program.h"
#include "Buffer.h"
#include <memory>

class GLSea
{
public:
    GLSea();
    void render(glm::vec3& eyePos, double time=0);
private:
    GLint vertexLocation{};
    void prepare_grid();
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
    std::unique_ptr<Buffer> ibo;
//    GLuint tex{};
//    GLuint normal_tex{};
//    GLuint spec_tex{};
    int m_viewLoc;
    int m_timeLoc;
    const GLuint gridsize{1000};
    const float size{100.0f};
};

#endif // GLSEA_H
