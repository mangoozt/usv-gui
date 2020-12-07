#include "glrestrictions.h"
#include "earcut.h"
#include <cmath>
#include <iostream>
#include <array>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "layout(location = 1) in vec4 position;\n"
        "out highp mat3 TBN;"
        "uniform mat4 m_view;\n"
        "out highp VERTEX_OUT{"
        "   vec3 highp FragPos;"
        "} vertex_out;"
        "void main() {\n"
        "   gl_Position = m_view * vertex;\n"
        "   vec3 Normal = vec3(0, 0, 1);\n"
        "   Normal = normalize(Normal);\n"
        "   vec3 Tangent = normalize(vec3(Normal.z,0,-Normal.y));"
        "   vec3 Tangent2 = normalize(vec3(0,Normal.z,-Normal.x));"
        "   TBN = mat3(Tangent,Tangent2,Normal);"
        "   vertex_out.FragPos=vertex.xyz;"
        "}\n";

static const char* fragmentShaderSource =
        "#version 330\n"
        //        "#extension GL_OES_standard_derivatives : enable\n"
        "struct Material {"
        "   vec3 ambient;"
        "   vec3 diffuse;"
        "   vec3 specular;"
        "   float shininess;"
        "};"
        "struct Light {"
        "   vec3 position;"
        "   vec3 ambient;"
        "   vec3 diffuse;"
        "   vec3 specular;"
        "};"
        "in highp mat3 TBN;\n"
        "out highp vec4 fragColor;\n"
        "uniform Light light; "
        "uniform highp vec3 viewPos;\n"
        "uniform Material material;\n"
        "in highp VERTEX_OUT{"
        "   vec3 highp FragPos;"
        "} vertex_out;"
        ""
        "void main() {\n"
        "   vec3 norm = normalize(TBN[2]);"
        // ambient
        "   vec3 ambient = light.ambient * material.ambient;\n"
        // diffuse
        "   vec3 lightDir = normalize(light.position - vertex_out.FragPos);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
        // specular
        "   vec3 viewDir = normalize(viewPos - vertex_out.FragPos);\n"
        "   vec3 reflectDir = reflect(-lightDir, norm);\n"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
        "   vec3 specular = light.specular * (spec * material.specular);\n"
        "   vec3 result = ambient + diffuse + specular;\n"
        "   fragColor = vec4(result, 1.0);\n"
        "}\n";

GLRestrictions::GLRestrictions() {
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_program->bind();
    m_viewMatrixLoc = m_program->uniformLocation("m_view");
    m_viewLoc = m_program->uniformLocation("viewPos");
    m_timeLoc = m_program->uniformLocation("time");
    m_program->setUniformValue(m_program->uniformLocation("height_scale"), 0.2f);
    struct {
        QVector3D position = QVector3D(-100.0, 100.0, 10.0);
        QVector3D ambient = QVector3D(0.7, 0.6, 0.6);
        QVector3D diffuse = QVector3D(0.7, 0.6, 0.6);
        QVector3D specular = QVector3D(1, 1, 1);
    } light;
    m_program->setUniformValue(m_program->uniformLocation("light.position"), light.position);
    m_program->setUniformValue(m_program->uniformLocation("light.ambient"), light.ambient);
    m_program->setUniformValue(m_program->uniformLocation("light.diffuse"), light.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("light.specular"), light.specular);

    struct {
        QVector3D ambient = QVector3D(255, 0, 0) / 255;
        QVector3D diffuse = QVector3D(255, 200, 0) / 255;
        QVector3D specular = QVector3D(255, 204, 51) / 400;
        float shininess{256};
    } material;
    m_program->setUniformValue(m_program->uniformLocation("material.ambient"), material.ambient);
    m_program->setUniformValue(m_program->uniformLocation("material.diffuse"), material.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("material.specular"), material.specular);
    m_program->setUniformValue(m_program->uniformLocation("material.shininess"), material.shininess);
    m_program->release();
}

GLRestrictions::~GLRestrictions() {
    delete m_program;
}

void GLRestrictions::render(QMatrix4x4& view_matrix, QVector3D eyePos) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDepthMask(GL_FALSE);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    m_program->setUniformValue(m_viewMatrixLoc, view_matrix);
    m_program->setUniformValue(m_viewLoc, eyePos);
    for (auto& poly:glpolygons) {
        poly.render(m_program);
    }
    for (auto& poly:glcontours) {
        poly.render(m_program);
    }
    m_program->release();
    f->glDisable(GL_BLEND);
    f->glDepthMask(GL_TRUE);
}

void GLRestrictions::load_restrictions(const USV::Restrictions::Restrictions& restrictions) {
    glpolygons.clear();
    glcontours.clear();
    QVector3D c_hard{1.0, 0.0, 0.0};
    for (auto& limitation:restrictions.hard.ZoneEnteringProhibitions()) {
        glpolygons.emplace_back(limitation.polygon, c_hard);
    }
    QVector3D c_soft{1.0, 0.8, 0.0};
    for (auto& limitation:restrictions.soft.ZoneEnteringProhibitions()) {
        glcontours.emplace_back(limitation.polygon, c_soft);
    }
    QVector3D c_movement{0.9, 0.9, 0.9};
    for (auto& limitation:restrictions.soft.MovementParametersLimitations()) {
        glcontours.emplace_back(limitation.polygon, c_movement);
    }
    for (auto& limitation:restrictions.hard.MovementParametersLimitations()) {
        glcontours.emplace_back(limitation.polygon, c_movement);
    }
}

void GLRestrictions::Polygon::render(QOpenGLShaderProgram* program) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    program->setUniformValue(program->uniformLocation("material.ambient"), color);
    program->setUniformValue(program->uniformLocation("material.diffuse"), color * 0.8);
    program->setUniformValue(program->uniformLocation("material.specular"), QVector3D(255,255,255) / 400);
    program->setUniformValue(program->uniformLocation("material.shininess"), 16);
    vbo->bind();
    ibo->bind();
    int vertexLocation = program->attributeLocation("vertex");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, GL_FALSE, 2, 0);
    f->glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    ibo->release();
    vbo->release();
}

namespace mapbox::util {

    template<>
    struct nth<0, USV::Vector2> {
        inline static auto get(const USV::Vector2& t) {
            return t.x();
        };
    };

    template<>
    struct nth<1, USV::Vector2> {
        inline static auto get(const USV::Vector2& t) {
            return t.y();
        };
    };

} // namespace mapbox

GLRestrictions::Polygon::Polygon(const USV::Restrictions::Polygon& polygon, const QVector3D& color) : color(color) {
    // The index type.
    using N = GLuint;
    using Point = std::array<GLfloat, 2>;
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<N> indices = mapbox::earcut<N>(polygon.rings);
    indices_count = indices.size();
    std::vector<Point> vertices;
    for (auto& ring:polygon.rings)
        for (auto& point:ring)
            vertices.push_back({(GLfloat) point.x(), (GLfloat) point.y()});

    vbo = new QOpenGLBuffer();
    ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    vbo->create();
    ibo->create();
    vbo->bind();
    vbo->allocate(vertices.data(), sizeof(Point) * vertices.size());
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(), sizeof(GLuint) * indices.size());
    ibo->release();
}

GLRestrictions::Polygon::~Polygon() {
    delete vbo;
    delete ibo;
}

GLRestrictions::Contour::Contour(const USV::Restrictions::Polygon& polygon, const QVector3D& color) : color(color) {
    // The index type.
    using N = GLuint;
    using Point = std::array<GLfloat, 2>;
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<N> indices = mapbox::earcut<N>(polygon.rings);
    std::vector<Point> vertices;
    for (auto& ring:polygon.rings) {
        start_ptrs.push_back(vertices.size());
        for (auto& point:ring)
            vertices.push_back({(GLfloat) point.x(), (GLfloat) point.y()});
    }
    start_ptrs.push_back(vertices.size());

    vbo = new QOpenGLBuffer();
    vbo->create();
    vbo->bind();
    vbo->allocate(vertices.data(), sizeof(Point) * vertices.size());
    vbo->release();
}

void GLRestrictions::Contour::render(QOpenGLShaderProgram* program) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    program->setUniformValue(program->uniformLocation("material.ambient"), color);
    program->setUniformValue(program->uniformLocation("material.diffuse"), color * 0.8);
    program->setUniformValue(program->uniformLocation("material.specular"), QVector3D(255,255,255) / 400);
    program->setUniformValue(program->uniformLocation("material.shininess"), 16);
    vbo->bind();
    f->glVertexAttribPointer(program->attributeLocation("vertex"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    vbo->release();
    f->glLineWidth(2.0f);
    for (size_t i = 0, j = 1; j < start_ptrs.size(); i = j++)
        f->glDrawArrays(GL_LINE_LOOP, start_ptrs[i], start_ptrs[j] - start_ptrs[i]);
}