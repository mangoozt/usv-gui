#include "glrestrictions.h"
#include "earcut.h"
#include "glgrid.h"
#include <cmath>
#include <array>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

static const char* vertexShaderSource =
        "#version 330\n"
        "layout(location = 0) in vec4 vertex;\n"
        "layout(location = 1) in vec3 normal;\n"
        "out highp mat3 TBN;"
        "uniform mat4 m_view;\n"
        "out highp VERTEX_OUT{"
        "   vec3 highp FragPos;"
        "} vertex_out;"
        "void main() {\n"
        "   gl_Position = m_view * vertex;\n"
        "   vec3 Normal = normalize(normal);\n"
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
        "uniform float opacity;\n"
        "in highp VERTEX_OUT{"
        "   vec3 highp FragPos;"
        "} vertex_out;"
        "vec4 xygrid(vec2 coord, vec4 color,vec4 gridcolor);\n"
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
        "   fragColor = xygrid(vertex_out.FragPos.xy, vec4(result, opacity), vec4(135,135,135,255)/255);\n"
        "}\n";

GLRestrictions::GLRestrictions() {
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, GLGrid::xyGridShaderSource);
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
    m_program->setUniformValue(m_program->uniformLocation("opacity"), 1.0f);
    m_program->release();
}

GLRestrictions::~GLRestrictions() {
    delete m_program;
}

void GLRestrictions::render(QMatrix4x4& view_matrix, QVector3D eyePos, GeometryType gtype) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    m_program->setUniformValue(m_viewMatrixLoc, view_matrix);
    m_program->setUniformValue(m_viewLoc, eyePos);
    if (gtype & GeometryTypes::Isle)
        for (auto& poly:glisles) {
            poly.render(m_program);
        }
    if (gtype & GeometryTypes::Polygon)
        for (auto& poly:glpolygons) {
            poly.render(m_program);
        }
    if (gtype & GeometryTypes::Contour)
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
        if (limitation._ptr->source_object_code == "LNDARE")
            glisles.emplace_back(limitation.polygon, c_hard);
        else
            glcontours.emplace_back(limitation.polygon, c_hard);
    }
    QVector3D c_soft{1.0, 0.8, 0.0};
    for (auto& limitation:restrictions.soft.ZoneEnteringProhibitions()) {
        if (limitation._ptr->source_object_code == "LNDARE")
            glisles.emplace_back(limitation.polygon, c_soft);
        else
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
    program->bind();
    program->setUniformValue(program->uniformLocation("material.ambient"), color);
    program->setUniformValue(program->uniformLocation("material.diffuse"), color * 0.8);
    program->setUniformValue(program->uniformLocation("material.specular"), QVector3D(255,255,255) / 400);
    program->setUniformValue(program->uniformLocation("material.shininess"), 16);
    program->setUniformValue(program->uniformLocation("opacity"), opacity);
    vbo->bind();
    ibo->bind();
    int vertexLocation = program->attributeLocation("vertex");
    program->enableAttributeArray(vertexLocation);
    int normalLocation = program->attributeLocation("normal");
    program->disableAttributeArray(normalLocation);
    program->setAttributeValue(normalLocation,QVector3D(0.0,0.0,1.0));
    f->glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    f->glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    ibo->release();
    vbo->release();
    program->disableAttributeArray(vertexLocation);
    program->setUniformValue(program->uniformLocation("opacity"), 1.0f);
    program->release();
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

GLRestrictions::Polygon::Polygon(const USV::Restrictions::Polygon& polygon, const QVector4D& color, float opacity)
        : color(color), opacity(opacity) {
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<Index> indices = mapbox::earcut<Index>(polygon.rings);
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

void GLRestrictions::Isle::render(QOpenGLShaderProgram* program) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    program->bind();
    program->setUniformValue(program->uniformLocation("material.ambient"), color*0.5);
    program->setUniformValue(program->uniformLocation("material.diffuse"), color);
    program->setUniformValue(program->uniformLocation("material.specular"), QVector3D(255, 255, 255) / 400);
    program->setUniformValue(program->uniformLocation("material.shininess"), 16);
    vbo->bind();
    ibo->bind();
    int vertexLocation = program->attributeLocation("vertex");
    int normLocation = program->attributeLocation("normal");
    f->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    program->enableAttributeArray(vertexLocation);
    f->glVertexAttribPointer(normLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
    program->enableAttributeArray(normLocation);

    f->glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    program->disableAttributeArray(vertexLocation);
    program->disableAttributeArray(normLocation);
    ibo->release();
    vbo->release();
    program->release();
}

GLRestrictions::Isle::Isle(const USV::Restrictions::Polygon& polygon, const QVector3D& color) : color(color) {
    using Point6 = std::array<GLfloat, 6>;
    const auto z = 0.2f;
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<Index> indices = mapbox::earcut<Index>(polygon.rings);

    std::vector<Point6> vertices;
    for (auto& ring:polygon.rings)
        for (auto& point:ring)
            vertices.push_back({(GLfloat) point.x(), (GLfloat) point.y(), z, 0, 0, 1});


    // build sidewall
    GLuint r = polygon.rings[0].size();
    auto s = vertices.size();
    for (GLuint i = 0; i < polygon.rings[0].size(); ++i) {
        USV::Vector2 d, d1;
        if (i == 0)
            d = polygon.rings[0][i] - polygon.rings[0][i - 1 + r];
        else
            d = polygon.rings[0][i] - polygon.rings[0][i - 1];

        if (i == r - 1)
            d1 = polygon.rings[0][0] - polygon.rings[0][i];
        else
            d1 = polygon.rings[0][i + 1] - polygon.rings[0][i];

        // prev
        // top
        vertices.push_back(
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), z,
                 static_cast<float>(-d.y()), static_cast<float>(d.x()), 0});
        // bottom
        vertices.push_back(
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), -5 * z,
                 static_cast<float>(-d.y()), static_cast<float>(d.x()), 0});
        // next
        //top
        vertices.push_back(
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), z,
                 static_cast<float>(-d1.y()), static_cast<float>(d1.x()), 0});
        //bottom
        vertices.push_back(
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), -5 * z,
                 static_cast<float>(-d1.y()), static_cast<float>(d1.x()), 0});
    }

    r=4*(r);
    for (GLuint j = 0; j < polygon.rings[0].size(); ++j) {
        // indices
        indices.push_back(s + 4 * j - 1 + r);
        indices.push_back(s + 4 * j - 2 + r);
        indices.push_back(s + 4 * j + 1);
        indices.push_back(s + 4 * j - 2 + r);
        indices.push_back(s + 4 * j);
        indices.push_back(s + 4 * j + 1);
        r = 0;
    }


    indices_count = indices.size();
    vbo = new QOpenGLBuffer();
    ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    vbo->create();
    ibo->create();
    vbo->bind();
    vbo->allocate(vertices.data(), sizeof(Point6) * vertices.size());
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(), sizeof(GLuint) * indices.size());
    ibo->release();

}

GLRestrictions::Isle::~Isle() {
    delete vbo;
    delete ibo;
}


GLRestrictions::Contour::Contour(const USV::Restrictions::Polygon& polygon, const QVector3D& color) : color(color) {
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
    program->bind();
    program->setUniformValue(program->uniformLocation("material.ambient"), color);
    program->setUniformValue(program->uniformLocation("material.diffuse"), color * 0.8);
    program->setUniformValue(program->uniformLocation("material.specular"), QVector3D(255,255,255) / 400);
    program->setUniformValue(program->uniformLocation("material.shininess"), 16);
    vbo->bind();
    int vertexLocation = program->attributeLocation("vertex");
    program->enableAttributeArray(vertexLocation);
    int normalLocation = program->attributeLocation("normal");
    program->setAttributeValue(normalLocation,QVector3D(0.0,0.0,1.0));
    f->glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    vbo->release();
    f->glLineWidth(3.0f);
    for (size_t i = 0, j = 1; j < start_ptrs.size(); i = j++)
        f->glDrawArrays(GL_LINE_LOOP, start_ptrs[i], start_ptrs[j] - start_ptrs[i]);

    program->disableAttributeArray(vertexLocation);
    program->release();
}