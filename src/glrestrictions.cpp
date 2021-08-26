#include "glrestrictions.h"
#include "earcut.h"
#include "utils.h"
#include "glgrid.h"
#include <array>
#include <cmrc/cmrc.hpp>
#include "Defines.h"
#include "Program.h"
#include "Buffer.h"

CMRC_DECLARE(glsl_resources);

GLRestrictions::GLRestrictions() {
    m_program = std::make_unique<Program>();
    auto fs = cmrc::glsl_resources::get_filesystem();
    m_program->addVertexShader(fs.open("glsl/general.vert").cbegin());
    m_program->addFragmentShader(GLGrid::xyGridShaderSource);
    m_program->addFragmentShader(fs.open("glsl/restrictions.frag").cbegin());
    m_program->link();
    m_program->bind();

    auto ul_matrices = glGetUniformBlockIndex(m_program->programId(), "Matrices");
    glUniformBlockBinding(m_program->programId(), ul_matrices, USV_GUI_MATRICES_BINDING);

    auto ul_light = glGetUniformBlockIndex(m_program->programId(), "Light");
    glUniformBlockBinding(m_program->programId(), ul_light, USV_GUI_LIGHTS_BINDING);

    m_viewLoc = m_program->uniformLocation("viewPos");

    const struct {
        glm::vec3 ambient = glm::vec3(255, 0, 0) / 255.0f;
        glm::vec3 diffuse = glm::vec3(255, 200, 0) / 255.0f;
        glm::vec3 specular = glm::vec3(255, 204, 51) / 400.0f;
        float shininess{256};
    } material;
    m_program->setUniformValue(m_program->uniformLocation("material.ambient"), material.ambient);
    m_program->setUniformValue(m_program->uniformLocation("material.diffuse"), material.diffuse);
    m_program->setUniformValue(m_program->uniformLocation("material.specular"), material.specular);
    m_program->setUniformValue(m_program->uniformLocation("material.shininess"), material.shininess);
    m_program->setUniformValue(m_program->uniformLocation("opacity"), 1.0f);
    m_program->release();
}

void GLRestrictions::render(glm::vec3 eyePos, GeometryType gtype) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_program->bind();
    m_program->setUniformValue(m_viewLoc, eyePos);
    glDepthMask(GL_TRUE);
    if (gtype & GeometryTypes::Isle)
        for (auto& poly:glisles) {
            poly.render(*m_program);
        }
    glDepthMask(GL_FALSE);
    if (gtype & GeometryTypes::Polygon)
        for (auto& poly:glpolygons) {
            poly.render(*m_program);
        }
    glDepthMask(GL_TRUE);
    if (gtype & GeometryTypes::Contour)
        for (auto& poly:glcontours) {
            poly.render(*m_program);
        }
    m_program->release();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void GLRestrictions::load_restrictions(const USV::Restrictions::Restrictions& restrictions) {
    glpolygons.clear();
    glcontours.clear();
    glisles.clear();
    glm::vec3 c_hard{1.0f, 0.0f, 0.0f};
    glm::vec3 c_soft{1.0f, 0.8f, 0.0f};
    for (auto& limitation:restrictions.hard.ZoneEnteringProhibitions()) {
        meta_.push_back({limitation._ptr});
        if (limitation._ptr->source_object_code == "LNDARE") {
            glisles.emplace_back(limitation.polygon, c_hard, meta_.size() - 1);
        }
        else { glpolygons.emplace_back(limitation.polygon, c_hard, meta_.size() - 1, 0.5f);
        glcontours.emplace_back(limitation.polygon, c_soft, meta_.size() - 1);}
    }

    for (auto& limitation:restrictions.soft.ZoneEnteringProhibitions()) {
        meta_.push_back({limitation._ptr});
        if (limitation._ptr->source_object_code == "LNDARE")
            glisles.emplace_back(limitation.polygon, c_soft, meta_.size() - 1);
        else { glcontours.emplace_back(limitation.polygon, c_soft, meta_.size() - 1); glcontours.emplace_back(limitation.polygon, c_soft, meta_.size() - 1); }
    }
    glm::vec3 c_movement{0.5f, 0.5f, 0.5f};
    for (auto& limitation:restrictions.soft.MovementParametersLimitations()) {
        meta_.push_back({limitation._ptr});
        glpolygons.emplace_back(limitation.polygon, c_movement, meta_.size() - 1);
        glcontours.emplace_back(limitation.polygon, c_soft, meta_.size() - 1);
    }
    for (auto& limitation:restrictions.hard.MovementParametersLimitations()) {
        meta_.push_back({limitation._ptr});
        glpolygons.emplace_back(limitation.polygon, c_movement, meta_.size() - 1);
        glcontours.emplace_back(limitation.polygon, c_soft, meta_.size() - 1);
    }
}

void GLRestrictions::Polygon::render(const Program& program) {
    program.bind();
    program.setUniformValue(program.uniformLocation("material.ambient"), color);
    program.setUniformValue(program.uniformLocation("material.diffuse"), color * 0.8f);
    program.setUniformValue(program.uniformLocation("material.specular"), glm::vec3(255, 255, 255) / 400.0f);
    program.setUniformValue(program.uniformLocation("material.shininess"), 16.0f);
    program.setUniformValue(program.uniformLocation("opacity"), opacity);
    glUniform1i(program.uniformLocation("_id"), (GLint) id_);
    vbo->bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->bufferId());
    int vertexLocation = glGetAttribLocation(program.programId(), "vertex");
    glEnableVertexAttribArray(vertexLocation);
    int normalLocation = glGetAttribLocation(program.programId(), "normal");
    glDisableVertexAttribArray(normalLocation);
    glVertexAttrib3f(normalLocation, 0.0, 0.0, 1.0);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    vbo->release();
    glDisableVertexAttribArray(vertexLocation);
    program.setUniformValue(program.uniformLocation("opacity"), 1.0f);
    glUseProgram(0);
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

GLRestrictions::Polygon::Polygon(const USV::Restrictions::Polygon& polygon, const glm::vec3& color, size_t id,
                                 float opacity)
        : color(color), opacity(opacity), id_(id) {
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<Index> indices = mapbox::earcut<Index>(polygon.rings);
    indices_count = static_cast<GLuint>(indices.size());
    std::vector<Point> vertices;
    for (auto& ring:polygon.rings)
        for (auto& point:ring)
            vertices.push_back({(GLfloat) point.x(), (GLfloat) point.y()});

    vbo = std::make_unique<Buffer>();
    ibo = std::make_unique<Buffer>();
    vbo->create();
    ibo->create();
    vbo->bind();
    vbo->allocate(vertices.data(), static_cast<int>(data_sizeof(vertices)));
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(), static_cast<int>(data_sizeof(indices)));
    ibo->release();
}

GLRestrictions::Polygon::~Polygon() = default;

GLRestrictions::Polygon::Polygon(GLRestrictions::Polygon&& o) noexcept:
        vbo(std::exchange(o.vbo, nullptr)), ibo(std::exchange(o.ibo, nullptr)), indices_count(o.indices_count)
        , color(o.color), opacity(o.opacity), id_(o.id_) {}

void GLRestrictions::Isle::render(const Program& program) {
    program.bind();
    program.setUniformValue(program.uniformLocation("material.ambient"), color * 0.5f);
    program.setUniformValue(program.uniformLocation("material.diffuse"), color);
    program.setUniformValue(program.uniformLocation("material.specular"), glm::vec3(255, 255, 255) / 400.0f);
    program.setUniformValue(program.uniformLocation("material.shininess"), 16);
    vbo->bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->bufferId());
    int vertexLocation = glGetAttribLocation(program.programId(), "vertex");
    int normLocation = glGetAttribLocation(program.programId(), "normal");
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) nullptr);

    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(normLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normLocation);

    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
    glDisableVertexAttribArray(vertexLocation);
    glDisableVertexAttribArray(normLocation);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    vbo->release();
    glUseProgram(0);
}

GLRestrictions::Isle::Isle(const USV::Restrictions::Polygon& polygon, const glm::vec3& color, size_t id) : color(color)
        , id_(id) {
    using Point6 = std::array<GLfloat, 6>;
    const auto z = 0.1f;
    // Run tessellation
    // Returns array of indices that refer to the vertices of the input polygon.
    // Three subsequent indices form a triangle. Output triangles are clockwise.
    std::vector<Index> indices = mapbox::earcut<Index>(polygon.rings);

    std::vector<Point6> vertices;
    for (auto& ring:polygon.rings)
        for (auto& point:ring)
            vertices.push_back({(GLfloat) point.x(), (GLfloat) point.y(), z, 0, 0, 1});


    // build sidewall
    auto r = static_cast<GLuint>(polygon.rings[0].size());
    auto s = static_cast<GLuint>(vertices.size());
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
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), 0,
                 static_cast<float>(-d.y()), static_cast<float>(d.x()), 0});
        // next
        //top
        vertices.push_back(
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), z,
                 static_cast<float>(-d1.y()), static_cast<float>(d1.x()), 0});
        //bottom
        vertices.push_back(
                {(GLfloat) polygon.rings[0][i].x(), (GLfloat) polygon.rings[0][i].y(), 0,
                 static_cast<float>(-d1.y()), static_cast<float>(d1.x()), 0});
    }

    r = 4 * (r);
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


    indices_count = static_cast<GLuint>(indices.size());
    vbo = std::make_unique<Buffer>();
    ibo = std::make_unique<Buffer>();
    vbo->create();
    ibo->create();
    vbo->bind();
    vbo->allocate(vertices.data(), static_cast<int>(data_sizeof(vertices)));
    vbo->release();
    ibo->bind();
    ibo->allocate(indices.data(), static_cast<int>(data_sizeof(indices)));
    ibo->release();

}

GLRestrictions::Isle::~Isle() = default;

GLRestrictions::Isle::Isle(GLRestrictions::Isle&& o) noexcept:
        vbo(std::exchange(o.vbo, nullptr)), ibo(std::exchange(o.ibo, nullptr)), indices_count(o.indices_count)
        , color(o.color), id_(o.id_) {}


GLRestrictions::Contour::Contour(const USV::Restrictions::Polygon& polygon, const glm::vec3& color, size_t id) : color(
        color), id_(id) {
    std::vector<Point> vertices;
    for (auto& ring:polygon.rings) {
        start_ptrs.push_back(static_cast<GLuint>(vertices.size()));
        for (auto& point:ring)
            vertices.push_back({(GLfloat) point.x(), (GLfloat) point.y()});
    }
    start_ptrs.push_back(static_cast<GLuint>(vertices.size()));

    vbo = std::make_unique<Buffer>();
    vbo->create();
    vbo->bind();
    vbo->allocate(vertices.data(), static_cast<int>(data_sizeof(vertices)));
    vbo->release();
}

void GLRestrictions::Contour::render(const Program& program) {
    program.bind();
    program.setUniformValue(program.uniformLocation("material.ambient"), color);
    program.setUniformValue(program.uniformLocation("material.diffuse"), color * 0.8f);
    program.setUniformValue(program.uniformLocation("material.specular"), glm::vec3(255, 255, 255) / 400.0f);
    program.setUniformValue(program.uniformLocation("material.shininess"), 16);
    vbo->bind();
    int vertexLocation = glGetAttribLocation(program.programId(), "vertex");
    glEnableVertexAttribArray(vertexLocation);
    int normalLocation = glGetAttribLocation(program.programId(), "normal");
    glVertexAttrib3f(normalLocation, 0.0, 0.0, 1.0);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    vbo->release();
    for (size_t i = 0, j = 1; j < start_ptrs.size(); i = j++)
        glDrawArrays(GL_LINE_LOOP, start_ptrs[i], start_ptrs[j] - start_ptrs[i]);

    glDisableVertexAttribArray(vertexLocation);
    glUseProgram(0);
}

GLRestrictions::Contour::Contour(GLRestrictions::Contour&& o) noexcept:
        vbo(std::exchange(o.vbo, nullptr)), start_ptrs(std::move(o.start_ptrs)), color(o.color), id_(o.id_) {}

GLRestrictions::Contour::~Contour() = default;
