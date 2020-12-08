#include "skybox.h"

#include "utils.h"
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec3 vertex;\n"
        "uniform mat4 m_view;\n"
        "uniform vec4 position;\n"
        "out vec3 TexCoords;\n"
        "void main()\n"
        "{\n"
        "   TexCoords = vertex;\n"
        "   gl_Position = m_view * vec4(vertex*position.w+vec3(position.xy,0.0), 1.0);\n"
        "}\n";

static const char* fragmentShaderSource =
        "#version 330\n"
        "struct Light {"
        "   vec3 position;"
        "   vec3 ambient;"
        "   vec3 diffuse;"
        "   vec3 specular;"
        "};"
        "out highp vec4 fragColor;\n"
        "uniform Light light;\n"
        "in vec3 TexCoords;\n"
        "void main() {\n"
        "   float a = (dot(normalize(light.position),normalize(TexCoords))+1.0)*0.5;\n"
        "   float sun = pow(a,1024);\n"
        "   float glow = pow(a,512);\n"
        "   float dusk = a;\n"
        "   vec3 color = light.specular*sun+light.ambient*glow+dusk*vec3(135, 206, 235)/255;\n"
        "   color *= step(0.0, TexCoords.z);\n"
        "   fragColor = vec4(color,1.0);\n"
        "}\n";

Skybox::Skybox() {
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();
    m_viewMatrixLoc = m_program->uniformLocation("m_view");
    struct {
        QVector3D position = QVector3D(-100.0f, 100.0f, 10.0f);
        QVector3D ambient = QVector3D(0.7f, 0.6f, 0.6f);
        QVector3D diffuse = QVector3D(0.7f, 0.6f, 0.6f);
        QVector3D specular = QVector3D(1, 1, 1);
    } light;
    m_program->setUniformValue(m_program->uniformLocation("light.position"), light.position);
    m_program->setUniformValue(m_program->uniformLocation("light.ambient"), light.ambient);
    m_program->setUniformValue(m_program->uniformLocation("light.diffuse"), light.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("light.specular"), light.specular);
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

void Skybox::render(QMatrix4x4& view_matrix, QVector3D eye) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDepthMask(GL_FALSE);
    f->glDepthFunc(GL_LEQUAL);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    m_program->setUniformValue(m_viewMatrixLoc, view_matrix);
    m_program->setUniformValue(m_program->uniformLocation("position"), eye.x(), eye.y(), eye.z(), 60.0f);
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
