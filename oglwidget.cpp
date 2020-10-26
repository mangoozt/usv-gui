#include "oglwidget.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <iostream>

OGLWidget::OGLWidget(QWidget *parent)
    :QOpenGLWidget(parent),
      m_texture(0),
      m_program(0),
      m_vbo(0),
      m_vao(0),
      m_eye(0,0,20),
      m_target(0, 0, 0),
      m_uniformsDirty(true)
{
    m_world.setToIdentity();
    //    m_world.translate(0, 0, 0);
    //    m_world.rotate(180, 1, 0, 0);
}

OGLWidget::~OGLWidget()
{
    makeCurrent();
    delete m_texture;
    delete m_program;
    delete m_vbo;
    delete m_vao;
    delete m_vessels;
    delete m_paths;
}

static const char *vertexShaderSource =
        "layout(location = 0) in vec4 vertex;\n"
        "layout(location = 1) in vec4 position;\n"
        "layout(location = 2) in float w;\n"
        "layout(location = 3) in vec3 col;\n"
        "out vec3 color;\n"
        "uniform mat4 myMatrix;\n"
        "void main() {\n"
        "   mat4 rot = mat4(cos(w),sin(w),0,0, -sin(w),cos(w),0,0, 0,0,1,0, 0,0,0,1);\n"
        "   mat4 translate = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, position.x,position.y,position.z,1);\n"
        "   color = col;\n"
        "   gl_Position = myMatrix *(translate*rot*vertex);\n"
        "}\n";

static const char *fragmentShaderSource =
        "in highp vec3 vert;\n"
        "in highp vec3 color;\n"
        "out highp vec4 fragColor;\n"
        "uniform highp vec3 lightPos;\n"
        "void main() {\n"
        "   fragColor = vec4(color,1.0);\n"
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
    auto *f = QOpenGLContext::currentContext()->extraFunctions();

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

    m_vessels = new QOpenGLBuffer;
    m_vessels->create();

    m_paths = new QOpenGLBuffer;
    m_paths->create();

    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);
}

void OGLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 200.0f);
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
        camera.lookAt(m_eye, m_target, QVector3D(0, 1, 0));
        m_m=m_proj * camera * m_world;
        m_program->setUniformValue(m_myMatrixLoc, m_m);
        m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));
    }

    // Draw vessels
    m_vbo->bind();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    f->glEnableVertexAttribArray(3);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    m_vbo->release();

    m_vessels->bind();

    f->glVertexAttribDivisor(1, 1);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 0*sizeof(float) ) );

    f->glVertexAttribDivisor(2, 1);
    f->glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 2*sizeof(float) ) );

    f->glVertexAttribDivisor(3, 1);
    f->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) ( 3*sizeof(float) ) );
    m_vessels->release();
    f->glLineWidth(1.0f);

    f->glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, case_data.vessels.size());

    // Draw route
    f->glDisableVertexAttribArray(1);
    f->glVertexAttrib4f(1, 0.0f,0.0f,0.0f,0.0f);
    f->glDisableVertexAttribArray(2);
    f->glVertexAttrib1f(2, 0.0f);
    f->glDisableVertexAttribArray(3);
    m_paths->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    m_paths->release();
    for(const auto& path_meta:m_paths_meta){
        f->glVertexAttrib3f(3, path_meta.color.x(),path_meta.color.y(),path_meta.color.z());
        f->glDrawArrays(GL_LINE_STRIP, path_meta.ptr, path_meta.points_count);
    }

}
#define PRINT_POINT_3D(point) std::cout <<#point<< ": "<<(point).x()<<", "<<(point).y()<<", "<<(point).z()<< std::endl;
#define PRINT_POINT_4D(point) std::cout <<#point<< ": "<<(point).x()<<", "<<(point).y()<<", "<<(point).z()<<", "<<(point).w() << std::endl;
QVector3D OGLWidget::screenToWorld(QPoint pos)
{
    auto minv=m_m.inverted();
    QVector4D point_normalized=QVector4D(pos.x()/(float)width()*2-1, 1-pos.y()/(float)height()*2, 0.0f, 1.0f);
    auto position = point_normalized*minv;
    return QVector3D(position.x()*position.w(),position.y()*position.w(),0);
}

void OGLWidget::loadData(USV::CaseData &caseData){
    case_data = caseData;

    updatePositions(case_data.vessels);

    std::vector<GLfloat> paths;
    auto path_points=case_data.route.getPointsPath();
    for(const auto& v: path_points){
        paths.push_back(v.x());
        paths.push_back(v.y());
    }
    m_paths_meta.clear();
    m_paths_meta.emplace_back(0,path_points.size(),QVector4D(0,0,1.0f,0));
    for(const auto &path:case_data.targets_maneuvers){
        path_points=path.getPointsPath();
        size_t ptr=paths.size()/2;
        for(const auto& v: path_points){
            paths.push_back(v.x());
            paths.push_back(v.y());
        }
        m_paths_meta.emplace_back(ptr,path_points.size(),QVector4D(0,0,0,0));
    }
    m_paths->bind();
    m_paths->allocate(paths.data(),sizeof(GLfloat)*paths.size());
    m_paths->release();
}

void OGLWidget::updatePositions(const std::vector<USV::Vessel>& vessels){
    case_data.vessels=vessels;
    std::vector<GLfloat> spos;
    for(const auto& v: vessels){
        spos.push_back(v.position.x());
        spos.push_back(v.position.y());
        spos.push_back(v.course);
        for(size_t i=0;i<3;++i)
            spos.push_back(v.color[i]);
    }
    m_vessels->bind();
    m_vessels->allocate(spos.data(),sizeof(GLfloat)*spos.size());
    m_vessels->release();
}



void OGLWidget::mousePressEvent(QMouseEvent *event){
    setCursor(Qt::ClosedHandCursor);
    std::cout << "clicked at postion: "<<event->x()<<", "<<event->y()<< std::endl;
    mouse_press_point = event->pos();
    PRINT_POINT_3D(screenToWorld(mouse_press_point))
}

void OGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
}


void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        auto diff= QPointF(mouse_press_point-event->pos());
        mouse_press_point = event->pos();

        diff.setX(diff.x()*2.0f/width());
        diff.setY(diff.y()*2.0f/height());

        float fov_rad = 45.0f * M_PI / 180.0;
        float tan_fov=tan(fov_rad / 2.0);
        auto r=m_eye.z();
        float xtrans = diff.x() * r * tan_fov * (width()/(float)height());
        float ytrans = -diff.y() * r * tan_fov;

        m_eye.setZ(std::clamp(m_eye.z(), 0.0f,200.0f));
        m_eye.setX(std::clamp(m_eye.x()+xtrans,-200.0f,200.0f));
        m_eye.setY(std::clamp(m_eye.y()+ytrans,-200.0f,200.0f));

        m_target.setX(m_eye.x());
        m_target.setY(m_eye.y());
        PRINT_POINT_3D(m_eye)
        m_uniformsDirty = true;
        update();
    }
}


void OGLWidget::wheelEvent ( QWheelEvent * event )
{
    auto nx = event->x()/(float)width()*2-1;
    auto ny = 1-event->y()/(float)height()*2;
    auto delta = event->delta()/240.0f;
    m_eye.setZ(std::clamp(m_eye.z() - delta, 0.0f,200.0f));
    m_eye.setX(std::clamp(m_eye.x() + delta*nx,-200.0f,200.0f));
    m_eye.setY(std::clamp(m_eye.y() + delta*ny,-200.0f,200.0f));
    m_target.setX(m_eye.x());
    m_target.setY(m_eye.y());
    PRINT_POINT_3D(m_eye)
    m_uniformsDirty = true;
    this->update();
}
void OGLWidget::keyPressEvent(QKeyEvent *event) {
    using namespace Qt;
    switch(event->key()){
    case Key_W:
        m_eye.setY(std::clamp(m_eye.y()+0.1f,-60.0f,60.0f));
        break;
    case Key_S:
        m_eye.setY(std::clamp(m_eye.y()-0.1f,-60.0f,60.0f));
        break;
    case Key_D:
        m_eye.setX(std::clamp(m_eye.x()+0.1f,-60.0f,60.0f));
        break;
    case Key_A:
        m_eye.setX(std::clamp(m_eye.x()-0.1f,-60.0f,60.0f));
        break;
    default:
        return;
    }
    std::cout << m_eye.x()<<", "<<m_eye.y()<<", "<<m_eye.z() << std::endl;
    m_target.setX(m_eye.x());
    m_target.setY(m_eye.y());
    m_uniformsDirty = true;
    this->update();
}
