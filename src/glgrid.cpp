#include "glgrid.h"
#include <iostream>

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "out vec3 vert;"
        "uniform mat4 m_view;\n"
        "void main() {\n"
        "   gl_Position = m_view *vertex;\n"
        "   vert = vertex.xyz;"
        "}\n";

const char* GLGrid::xyGridShaderSource =
        "#version 330\n"
        "#extension GL_OES_standard_derivatives : enable\n"
        "vec4 xygrid(vec2 coord, vec4 color,vec4 gridcolor) {\n"
        "   vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);\n"
        "   vec2 rgrid = abs(fract(coord) - 0.5) / fwidth(coord);\n"
        "   float rline = min(rgrid.x, rgrid.y);\n"
        "   float line = min(grid.x, grid.y);"
        "   return mix(color,gridcolor,1.0 - min(min(rline*1.5,line), 1.0));\n"
        "}\n";

static const char* fragmentShaderSource =
        "#version 330\n"
        "#extension GL_OES_standard_derivatives : enable\n"
        "in highp vec3 vert;\n"
        "out highp vec4 fragColor;\n"
        "uniform highp vec4 color;\n"
        "uniform highp vec4 bg_color;\n"
        "vec4 xygrid(vec2 coord, vec4 color,vec4 gridcolor);\n"
        "void main() {\n"
        "   fragColor = xygrid(vert.xy,bg_color,color);\n"
        "}\n";

GLGrid::GLGrid() {
    m_program = std::make_unique<Program>();
    m_program->addVertexShader(vertexShaderSource);
    m_program->addFragmentShader(xyGridShaderSource);
    m_program->addFragmentShader(fragmentShaderSource);
    m_program->link();
    m_program->bind();
    m_viewMatrixLoc = m_program->uniformLocation("m_view");
    m_colorLoc = m_program->uniformLocation("color");
    m_program->setUniformValue(m_colorLoc, glm::vec4(135, 206, 250, 120) / 255.0f);
    m_program->setUniformValue(m_program->uniformLocation("bg_color"), glm::vec4(1.0, 1.0, 1.0, 0.0));
    m_program->release();
    vbo = std::make_unique<Buffer>();
    GLfloat plane[] = {
            -40.0f, 40.0f, -0.01f,
            -40.0f, -40.0f, -0.01f,
            40.0f, -40.0f, -0.01f,
            40.0f, 40.0f, -0.01f,
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
    m_program->setUniformValue(m_viewMatrixLoc, view_matrix);
    vbo->bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(0);
    vbo->release();
    m_program->release();
    glDisable(GL_BLEND);
}
