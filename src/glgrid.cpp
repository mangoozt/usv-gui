#include "glgrid.h"
#include <iostream>
#include "Program.h"
#include "Buffer.h"
#include "Defines.h"

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "out vec2 vert;"
        "out mat3 invmat;"
        "layout (std140) uniform Matrices\n"
        "{\n"
        "    mat4 projection;\n"
        "    mat4 view;\n"
        "};\n"
        "void main() {\n"
        "   mat4 c = projection*view;\n"
        "   invmat = inverse(mat3(c[0].xyw,c[1].xyw,c[3].xyw));\n"
        "   vert = vertex.xy;"
        "   gl_Position = vertex;"
        "}\n";

const char* GLGrid::xyGridShaderSource =
        "#version 330\n"
        "#extension GL_OES_standard_derivatives : enable\n"
        "vec4 xygrid(vec2 coord, vec4 color,vec4 gridcolor) {\n"
        "   vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);\n"
        "   vec2 rgrid = abs(fract(coord) - 0.5) / fwidth(coord);\n"
        "   float rline = min(rgrid.x, rgrid.y);\n"
        "   float line = min(grid.x, grid.y);"
        "   return mix(color,gridcolor,1.0 - min(min(rline,line*0.5), 1.0));\n"
        "}\n";

static const char* fragmentShaderSource =
        "#version 330\n"
        "#extension GL_OES_standard_derivatives : enable\n"
        "in highp vec2 vert;\n"
        "in mat3 invmat;"
        "out highp vec4 fragColor;\n"
        "uniform highp vec4 color;\n"
        "uniform highp vec4 bg_color;\n"
        "vec4 xygrid(vec2 coord, vec4 color,vec4 gridcolor);\n"
        "void main() {\n"
        "   vec3 v = invmat*vec3(vert,1);\n"
        "   v /= v.z;"
        "   vec2 dv = fwidth(v.xy);"
        "   fragColor = xygrid(v.xy,bg_color,color)*vec4(vec3(1.0),min(1.0,1/length(dv)));\n"
        "}\n";

GLGrid::GLGrid() {
    m_program = std::make_unique<Program>();
    m_program->addVertexShader(vertexShaderSource);
    m_program->addFragmentShader(xyGridShaderSource);
    m_program->addFragmentShader(fragmentShaderSource);
    m_program->link();
    m_program->bind();
    auto ul_matrices = glGetUniformBlockIndex(m_program->programId(), "Matrices");
    glUniformBlockBinding(m_program->programId(), ul_matrices, USV_GUI_MATRICES_BINDING);
    m_colorLoc = m_program->uniformLocation("color");
    m_program->setUniformValue(m_colorLoc, glm::vec4(135, 206, 250, 120) / 255.0f);
    m_program->setUniformValue(m_program->uniformLocation("bg_color"), glm::vec4(1.0, 1.0, 1.0, 0.0));
    m_program->release();
    vbo = std::make_unique<Buffer>();
    GLfloat plane[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -0.0f,
            1.0f, 1.0f, -0.0f,
    };
    vbo->create();
    vbo->bind();
    vbo->allocate(plane, sizeof(plane));
    vbo->release();
}

void GLGrid::render(const glm::mat4& view_matrix) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    vbo->bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(0);
    vbo->release();
    m_program->release();
    glDisable(GL_BLEND);
}
