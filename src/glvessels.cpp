#include "glvessels.h"
#include "earcut.h"
#include "utils.h"
#include <cmath>
#include <cmrc/cmrc.hpp>
#include "Defines.h"
#include "Program.h"
#include "Buffer.h"

#define CIRCLE_POINTS_N 360
const glm::vec3 vessel_vertices[] = {
        {-0.43301270189f * 0.1f, 0.0f,         0.01f},
        {0.43301270189f * 0.2f,  0.0f,         0.0f},
        {-0.43301270189f * 0.2f, 0.3f * 0.2f,  0.0f},

        {-0.43301270189f * 0.1f, 0.0f,         0.01f},
        {-0.43301270189f * 0.2f, 0.3f * 0.2f,  0.0f},
        {-0.43301270189f * 0.2f, -0.3f * 0.2f, 0.0f},

        {-0.43301270189f * 0.1f, 0.0f,         0.01f},
        {-0.43301270189f * 0.2f, -0.3f * 0.2f, 0.0f},
        {0.43301270189f * 0.2f,  0.0f,         0.0f},
};
static_assert((sizeof(vessel_vertices) / sizeof(glm::vec3)) % 3 == 0,
              "vessel_vertices isn't multiple of VESSEL_POINT_COMPONENTS_N");

CMRC_DECLARE(glsl_resources);

GLVessels::GLVessels() {
    m_program = std::make_unique<Program>();
    auto fs = cmrc::glsl_resources::get_filesystem();
    m_program->addVertexShader(fs.open("glsl/vessels.vert").cbegin());
    m_program->addFragmentShader(fs.open("glsl/vessels.frag").cbegin());
    m_program->link();
    m_program->bind();

    auto ul_matrices = glGetUniformBlockIndex(m_program->programId(), "Matrices");
    glUniformBlockBinding(m_program->programId(), ul_matrices, USV_GUI_MATRICES_BINDING);

    auto ul_light = glGetUniformBlockIndex(m_program->programId(), "Light");
    glUniformBlockBinding(m_program->programId(), ul_light, USV_GUI_LIGHTS_BINDING);

    m_viewLoc = m_program->uniformLocation("viewPos");

    m_program->setUniformValue(m_program->uniformLocation("opacity"), 1.0f);
    m_program->release();

    m_vessels = std::make_unique<Buffer>();
    m_vessels->create();

    m_vessel_vbo = std::make_unique<Buffer>();
    m_vessel_vbo->create();
    m_vessel_vbo->bind();
    {
        std::vector<glm::vec3> v_vessels;
        v_vessels.reserve(2 * (sizeof(vessel_vertices) / sizeof(glm::vec3)));
        for (size_t i = 0; i < (sizeof(vessel_vertices) / sizeof(glm::vec3) / 3); ++i) {
            const glm::vec3& p1 = vessel_vertices[i * 3];
            const glm::vec3& p2 = vessel_vertices[i * 3 + 1];
            const glm::vec3& p3 = vessel_vertices[i * 3 + 2];
            const glm::vec3 U = p2 - p1;
            const glm::vec3 V = p3 - p1;

            glm::vec3 Normal{
                    (U.y * V.z) - (U.z * V.y),
                    (U.z * V.x) - (U.x * V.z),
                    (U.x * V.y) - (U.y * V.x)
            };
            Normal = glm::normalize(Normal);
//            const glm::vec3 Normal{0,1,0};

            v_vessels.push_back(p1);
            v_vessels.push_back(Normal);
            v_vessels.push_back(p2);
            v_vessels.push_back(Normal);
            v_vessels.push_back(p3);
            v_vessels.push_back(Normal);
        }
        m_vessel_vbo->allocate(v_vessels.data(), (GLsizei) (sizeof(glm::vec3) * v_vessels.size()));
    }
    m_vessel_vbo->release();

    {
        std::vector<GLfloat> circles;
        size_t circle_N{CIRCLE_POINTS_N};
        circles.reserve(circle_N * 2);
        float x{1};
        float y{0};
        auto sin_ = std::sin((float) M_2PI / (float) circle_N);
        auto cos_ = std::cos((float) M_2PI / (float) circle_N);
        for (size_t i = 0; i < circle_N; ++i) {
            circles.push_back(x);
            circles.push_back(y);
            auto x_ = x;
            x = cos_ * x - sin_ * y;
            y = sin_ * x_ + cos_ * y;
        }
        m_circle_vbo = std::make_unique<Buffer>();
        m_circle_vbo->create();
        m_circle_vbo->bind();
        m_circle_vbo->allocate(circles.data(), (int) (sizeof(GLfloat) * circles.size()));
        m_circle_vbo->release();
    }
}

void GLVessels::render(glm::vec3 eyePos) {
    m_program->setUniformValue(m_viewLoc, eyePos);
    // Draw vessels
    m_vessel_vbo->bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
//    glVertexAttrib3f(1, 0.0f,0.0f,1.0f);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(5);
    // layout(location = 0) in vec4 vertex;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    // layout(location = 1) in vec3 normal;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
    m_vessel_vbo->release();
    m_vessels->bind();

    glVertexAttribDivisor(2, 1); // layout(location = 2) in vec4 position;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) nullptr);

    glVertexAttribDivisor(3, 1); // layout(location = 3) in float w;
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (2 * sizeof(float)));

    glVertexAttribDivisor(5, 1); // layout(location = 5) in vec3 color;
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (3 * sizeof(float)));

    glVertexAttrib1f(4, 1.0f); //layout(location = 4) in float scale;
    glLineWidth(1.0f);
    auto instancecount = (GLsizei) (vessels.size());
    if (case_data_ != nullptr) instancecount += (GLsizei) case_data_->targets.size() + 1;
    glDrawArraysInstanced(GL_TRIANGLES, 0, (sizeof(vessel_vertices) / sizeof(glm::vec3)), instancecount);
    m_vessels->release();

    // Circle
    m_circle_vbo->bind();
    // layout(location = 0) in vec4 vertex;
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDisableVertexAttribArray(1);
    glVertexAttrib3f(1, 0.0f, 0.0f, 1.0f);
    m_circle_vbo->release();
    m_vessels->bind();
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    // layout(location = 4) in float scale;
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (6 * sizeof(float)));
    glDrawArraysInstanced(GL_LINE_LOOP, 0, CIRCLE_POINTS_N, instancecount);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glVertexAttribDivisor(3, 0);
    glVertexAttribDivisor(4, 0);
    glVertexAttribDivisor(5, 0);
    m_vessels->release();

}

void GLVessels::updatePositions(const std::vector<Vessel>& new_vessels) {
    vessels = new_vessels;
    updatePositions();
}

void GLVessels::updatePositions() {
    std::vector<GLfloat> spos;
    double radius = 0;
    for (const auto& v: vessels) {
        auto color = appearance_settings.vessels_colors[static_cast<size_t>(v.type)];
        radius = v.radius;
        spos.push_back(static_cast<GLfloat>(v.position.x()));
        spos.push_back(static_cast<GLfloat>(v.position.y()));
        spos.push_back(static_cast<GLfloat>(v.course));
        spos.push_back(color.r);
        spos.push_back(color.g);
        spos.push_back(color.b);
        spos.push_back(static_cast<GLfloat>(v.radius));
    }
    if (case_data_ != nullptr) {
        auto color = appearance_settings.vessels_colors[static_cast<size_t>(Vessel::Type::TargetInitPosition)];
        for (const auto& t: case_data_->targets) {
            spos.push_back(static_cast<GLfloat>(t.initPosition.point.x()));
            spos.push_back(static_cast<GLfloat>(t.initPosition.point.y()));
            spos.push_back(static_cast<GLfloat>(t.initPosition.course.radians()));
            spos.push_back(color.r);
            spos.push_back(color.g);
            spos.push_back(color.b);
            spos.push_back(static_cast<GLfloat>(radius));
        }

        color = appearance_settings.vessels_colors[static_cast<size_t>(Vessel::Type::ShipInitPosition)];
        {
            const auto& ownShip = case_data_->ownShip;
            spos.push_back(static_cast<GLfloat>(ownShip.initPosition.point.x()));
            spos.push_back(static_cast<GLfloat>(ownShip.initPosition.point.y()));
            spos.push_back(static_cast<GLfloat>(ownShip.initPosition.course.radians()));
            spos.push_back(color.r);
            spos.push_back(color.g);
            spos.push_back(color.b);
            spos.push_back(static_cast<GLfloat>(radius));
        }
    }
    m_vessels->bind();
    m_vessels->allocate(spos.data(), (int) (sizeof(GLfloat) * spos.size()));
    m_vessels->release();
}
