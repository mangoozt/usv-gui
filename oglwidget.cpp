#include "oglwidget.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>

OGLWidget::OGLWidget(QWidget *parent)
    :QOpenGLWidget(parent),
      m_texture(0),
      m_program(0),
      m_vbo(0),
      m_vao(0),
      m_target(0, 0, -1),
      m_uniformsDirty(true)
{
    m_world.setToIdentity();
    m_world.translate(0, 0, -1);
    m_world.rotate(180, 1, 0, 0);
}

OGLWidget::~OGLWidget()
{
    makeCurrent();
    delete m_texture;
    delete m_program;
    delete m_vbo;
    delete m_vao;
}

static const char *vertexShaderSource =
    "layout(location = 0) in vec4 vertex;\n"
    "out vec3 color;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 camMatrix;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 myMatrix;\n"
    "void main() {\n"
    "   color = vec3(0.031372549, 0.282352941, 0.282352941);\n"
    "   gl_Position = projMatrix * camMatrix * worldMatrix * myMatrix * (vertex + vec4(gl_InstanceID*2,0,0,0));\n"
    "}\n";

static const char *fragmentShaderSource =
    "in highp vec3 vert;\n"
    "in highp vec3 color;\n"
    "out highp vec4 fragColor;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * L, 0.0, 1.0);\n"
    "   fragColor = vec4(col, 1.0);\n"
    "}\n";

QByteArray versionedShaderCode(const char *src)
{
    QByteArray versionedSrc;

    if (QOpenGLContext::currentContext()->isOpenGLES())
        versionedSrc.append(QByteArrayLiteral("#version 300 es\n"));
    else
        versionedSrc.append(QByteArrayLiteral("#version 330\n"));

    versionedSrc.append(src);
    return versionedSrc;
}

void OGLWidget::initializeGL()
{
     QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    if (m_texture) {
        delete m_texture;
        m_texture = 0;
    }

    if (m_program) {
        delete m_program;
        m_program = 0;
    }
    m_program = new QOpenGLShaderProgram;
    // Prepend the correct version directive to the sources. The rest is the
    // same, thanks to the common GLSL syntax.
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, versionedShaderCode(vertexShaderSource));
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, versionedShaderCode(fragmentShaderSource));
    m_program->link();

    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_camMatrixLoc = m_program->uniformLocation("camMatrix");
    m_worldMatrixLoc = m_program->uniformLocation("worldMatrix");
    m_myMatrixLoc = m_program->uniformLocation("myMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    // Create a VAO. Not strictly required for ES 3, but it is for plain OpenGL.
    if (m_vao) {
        delete m_vao;
        m_vao = 0;
    }
    m_vao = new QOpenGLVertexArrayObject;
    if (m_vao->create())
        m_vao->bind();

    if (m_vbo) {
        delete m_vbo;
        m_vbo = 0;
    }
    m_program->bind();
    m_vbo = new QOpenGLBuffer;
    m_vbo->create();
    m_vbo->bind();
    GLfloat ship[]={
        0.0f,0.3f,0.0f,
        0.75f,0.3f,0.0f,
        1.0f,0.0f,0.0f,
        0.75f,-0.3f,0.0f,
        0.0f,-0.3f,0.0f,
    };
    m_vbo->allocate(ship, sizeof(ship));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    m_vbo->release();

    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);
}

void OGLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
    m_uniformsDirty = true;
}

void OGLWidget::paintGL()
{
    // Now use QOpenGLExtraFunctions instead of QOpenGLFunctions as we want to
    // do more than what GL(ES) 2.0 offers.
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    f->glClearColor(0.984313725f, 0.984313725f, 0.984313725f, 1);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    if (m_uniformsDirty) {
        m_uniformsDirty = false;
        QMatrix4x4 camera;
        camera.lookAt(QVector3D(0,0,1), m_eye + m_target, QVector3D(0, 1, 0));
        m_program->setUniformValue(m_projMatrixLoc, m_proj);
        m_program->setUniformValue(m_camMatrixLoc, camera);
        m_program->setUniformValue(m_worldMatrixLoc, m_world);
        QMatrix4x4 mm;
        mm.setToIdentity();
        mm.translate(QVector3D(-20,0,20));
        m_program->setUniformValue(m_myMatrixLoc, mm);
        m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));
    }

    // Now call a function introduced in OpenGL 3.1 / OpenGL ES 3.0. We
    // requested a 3.3 or ES 3.0 context, so we know this will work.
    f->glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, 10);
}
