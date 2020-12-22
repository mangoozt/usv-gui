#include "skybox.h"
#include "utils.h"
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec3 vertex;\n"
        "layout (std140) uniform Matrices\n"
        "{\n"
        "    mat4 projection;\n"
        "    mat4 view;\n"
        "};\n"
        "out vec3 TexCoords;\n"
        "void main()\n"
        "{\n"
        "   TexCoords = vertex;\n"
        "   mat4 view_ = view; view_[3]=vec4(0);\n"
        "   gl_Position = projection*view_ * vec4(vertex, 1.0);\n"
        "}\n";

static const char* fragmentShaderSource =
        "#version 330\n"
        "layout (std140) uniform Light\n"
        "{\n"
        "    vec4 light_position;\n"
        "    vec3 light_ambient;\n"
        "    vec3 light_diffuse;\n"
        "    vec3 light_specular;\n"
        "};\n"
        "out highp vec4 fragColor;\n"
        "in vec3 TexCoords;\n"
        "void main() {\n"
        "   float a = (dot(normalize(light_position.xyz),normalize(TexCoords))+1.0)*0.5;\n"
        "   float sun = pow(a,1024);\n"
        "   float glow = pow(a,512);\n"
        "   float dusk = a;\n"
        "   vec3 color = light_specular*sun+light_ambient*glow+dusk*vec3(135, 206, 235)/255;\n"
        "   fragColor = vec4(color,1.0);\n"
        "}\n";

Skybox::Skybox() {
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();

    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    auto ul_matrices = m_program->uniformLocation("Matrices");
    f->glUniformBlockBinding(m_program->programId(), ul_matrices, 0);

    auto ul_light = f->glGetUniformBlockIndex(m_program->programId(), "Light");
    f->glUniformBlockBinding(m_program->programId(), ul_light, 1);
    m_program->release();

    float skyboxVertices[] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    vbo = new QOpenGLBuffer();
    vbo->create();
    vbo->bind();
    vbo->allocate(skyboxVertices, sizeof(skyboxVertices));
    vbo->release();
}

Skybox::~Skybox() {
    delete m_program;
    delete vbo;
}

void Skybox::render() {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDepthMask(GL_FALSE);
    f->glDepthFunc(GL_LEQUAL);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    vbo->bind();
    int vertexLocation = m_program->attributeLocation("vertex");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, GL_FALSE, 3, 0);
    f->glDrawArrays(GL_TRIANGLES, 0, 36);
    m_program->release();
    f->glDisable(GL_BLEND);
    f->glDepthMask(GL_TRUE);
    f->glDepthFunc(GL_LESS);
}
