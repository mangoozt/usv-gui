#include <cmath>
#include "glsea.h"
#include "utils.h"
#include <cmrc/cmrc.hpp>
#include "Defines.h"
#include "Program.h"
#include "Buffer.h"

CMRC_DECLARE(glsl_resources);

GLSea::GLSea() {
    m_program = std::make_unique<Program>();
    auto fs = cmrc::glsl_resources::get_filesystem();
    m_program->addVertexShader(fs.open("glsl/glsea.vert").begin());
    m_program->addFragmentShader(fs.open("glsl/glsea.frag").begin());
    m_program->link();
    m_program->bind();

    auto ul_matrices = glGetUniformBlockIndex(m_program->programId(), "Matrices");
    glUniformBlockBinding(m_program->programId(), ul_matrices, USV_GUI_MATRICES_BINDING);

    auto ul_light = glGetUniformBlockIndex(m_program->programId(), "Light");
    glUniformBlockBinding(m_program->programId(), ul_light, USV_GUI_LIGHTS_BINDING);

    m_viewLoc = m_program->uniformLocation("viewPos");
    m_timeLoc = m_program->uniformLocation("time");
    m_program->setUniformValue(m_program->uniformLocation("height_scale"), 0.2f);

    struct {
        glm::vec3 ambient = glm::vec3(127, 205, 255) / 255.0f;
        glm::vec3 diffuse = glm::vec3(127, 205, 255) / 255.0f;
        glm::vec3 specular = glm::vec3(255, 204, 51) / 400.0f;
        float shininess{256};
    } material;
    m_program->setUniformValue(m_program->uniformLocation("material.ambient"), material.ambient);
    m_program->setUniformValue(m_program->uniformLocation("material.diffuse"), material.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("material.specular"), material.specular);
    m_program->setUniformValue(m_program->uniformLocation("material.shininess"), material.shininess);
    glUniform1i(m_program->uniformLocation("tex_normal"), 0);
    glUniform1i(m_program->uniformLocation("depthMap"), 2);
    glUniform1i(m_program->uniformLocation("specularMap"), 4);
    vertexLocation = glGetAttribLocation(m_program->programId(), "vertex");
    m_program->release();
    vbo = std::make_unique<Buffer>();
    ibo = std::make_unique<Buffer>();
    vbo->create();
    ibo->create();
    prepare_grid();
}

void GLSea::render(glm::vec3& eyePos, double time) {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
//    normal_tex.bind(0);
//    tex.bind(2);
//    spec_tex.bind(4);
    m_program->setUniformValue(m_timeLoc, (float) std::fmod(time, 10) * 10.0f);
    m_program->setUniformValue(m_viewLoc, eyePos);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->bufferId());
    vbo->bind();
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);
    glDrawElements(GL_TRIANGLES, (gridsize - 1) * (gridsize - 1) * 6, GL_UNSIGNED_INT, nullptr);
    glDisableVertexAttribArray(vertexLocation);
    vbo->release();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_program->release();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void GLSea::prepare_grid() {
    std::vector<GLfloat> vertices;
    vertices.reserve(gridsize * gridsize * 2);
    auto step = size / gridsize;
    auto zero = -size * 0.5f;
    for (size_t i = 0; i < gridsize; ++i)
        for (size_t j = 0; j < gridsize; ++j) {
            vertices.push_back(zero + i * step);
            vertices.push_back(zero + j * step);
        }

    std::vector<GLuint> indices;
    for (GLuint i = 0; i < gridsize - 1; ++i)
        for (GLuint j = 0; j < gridsize - 1; ++j) {
            // first triangle
            auto r = i * gridsize;
            indices.push_back(r + j + gridsize);
            indices.push_back(r + j + 1);
            indices.push_back(r + j);
            // second triangle
            indices.push_back(r + j + gridsize);
            indices.push_back(r + j + gridsize + 1);
            indices.push_back(r + j + 1);
        }

    vbo->bind();
    vbo->allocate(vertices.data(), static_cast<int>(data_sizeof(vertices)));
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(), static_cast<int>(data_sizeof(indices)));
    ibo->release();
}
