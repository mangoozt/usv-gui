#include <cmath>
#include "glsea.h"
#include "utils.h"
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

GLSea::GLSea(QImage& texture, QImage& normal, QImage& specular) : tex(texture), normal_tex(normal), spec_tex(specular) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "glsl/glsea.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "glsl/glsea.frag");
    m_program->link();
    m_program->bind();

    auto ul_matrices = f->glGetUniformBlockIndex(m_program->programId(),"Matrices");
    f->glUniformBlockBinding(m_program->programId(), ul_matrices, 0);

    auto ul_light = f->glGetUniformBlockIndex(m_program->programId(), "Light");
    f->glUniformBlockBinding(m_program->programId(), ul_light, 1);

    m_viewLoc = m_program->uniformLocation("viewPos");
    m_timeLoc = m_program->uniformLocation("time");
    m_program->setUniformValue(m_program->uniformLocation("height_scale"), 0.2f);

    struct {
        QVector3D ambient = QVector3D(127, 205, 255) / 255;
        QVector3D diffuse = QVector3D(127, 205, 255) / 255;
        QVector3D specular = QVector3D(255, 204, 51) / 400;
        float shininess{256};
    } material;
    m_program->setUniformValue(m_program->uniformLocation("material.ambient"), material.ambient);
    m_program->setUniformValue(m_program->uniformLocation("material.diffuse"), material.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("material.specular"), material.specular);
    m_program->setUniformValue(m_program->uniformLocation("material.shininess"), material.shininess);
    f->glUniform1i(m_program->uniformLocation("tex_normal"), 0);
    f->glUniform1i(m_program->uniformLocation("depthMap"), 2);
    f->glUniform1i(m_program->uniformLocation("specularMap"), 4);
    m_program->release();
    vbo = new QOpenGLBuffer();
    ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    vbo->create();
    ibo->create();
    prepare_grid();
}

GLSea::~GLSea() {
    delete m_program;
    delete vbo;
    delete ibo;
}

void GLSea::render(QMatrix4x4& view_matrix, QVector3D eyePos, double time) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDepthMask(GL_FALSE);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    normal_tex.bind(0);
    tex.bind(2);
    spec_tex.bind(4);
    m_program->setUniformValue(m_timeLoc, (float) std::fmod(time, 10) * 10.0f);
    m_program->setUniformValue(m_viewLoc, eyePos);
    vbo->bind();
    ibo->bind();
    int vertexLocation = m_program->attributeLocation("vertex");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, GL_FALSE, 2, 0);
    f->glDrawElements(GL_TRIANGLES, (gridsize - 1) * (gridsize - 1) * 6, GL_UNSIGNED_INT, nullptr);
    ibo->release();
    m_program->release();
    f->glDisable(GL_BLEND);
    f->glDepthMask(GL_TRUE);
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
