#include "glpaths.h"
#include "Buffer.h"
#include "Program.h"
#include "Defines.h"

#define PATH_POINT_MARK_N 5

const std::string GlPaths::vertexShaderSource =
    "#version 330\n"
    "layout(location = 0) in vec4 vertex;\n"
    "layout(location = 1) in vec4 position;\n"
    "layout(location = 2) in float w;\n"
    "layout(location = 3) in vec3 col;\n"
    "layout(location = 4) in float scale;\n"
    "out vec3 color;\n"
    "layout (std140) uniform Matrices\n"
    "{\n"
    "    mat4 projection;\n"
    "    mat4 view;\n"
    "};\n"
    "void main() {\n"
    "   mat4 c = projection*view;\n"
    "   mat4 rot = mat4(cos(w),sin(w),0,0, -sin(w),cos(w),0,0, 0,0,1,0, "
    "0,0,0,1);\n"
    "   mat4 translate = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, "
    "position.x,position.y,position.z,1);\n"
    "   mat4 m_scale = mat4(scale,0,0,0, 0,scale,0,0, 0,0,scale,0, 0,0,0,1);\n"
    "   color = col;\n"
    "   gl_Position = c *(translate*rot*m_scale*vertex);\n"
    "}\n";

const std::string GlPaths::fragmentShaderSource =
    "#version 330\n"
    "in highp vec3 vert;\n"
    "in highp vec3 color;\n"
    "out highp vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(color,1.0);\n"
    "}\n";


GlPaths::GlPaths()
{
    m_program = std::make_unique<Program>();
    m_program->addVertexShader(vertexShaderSource.c_str());
    m_program->addFragmentShader(fragmentShaderSource.c_str());
    m_program->link();
    m_program->bind();
    auto ul_matrices = glGetUniformBlockIndex(m_program->programId(), "Matrices");
    glUniformBlockBinding(m_program->programId(), ul_matrices,
                            USV_GUI_MATRICES_BINDING);
    m_program->release();
    vbo = std::make_unique<Buffer>();
    vbo->create();
}

void GlPaths::initVbo(const std::vector<USV::Path>& paths)
{
    std::vector<GLfloat> pathsData;
    pathsData.push_back(static_cast<float>(0));
    pathsData.push_back(static_cast<float>(-1));
    pathsData.push_back(static_cast<float>(0));
    pathsData.push_back(static_cast<float>(-0.3));
    pathsData.push_back(static_cast<float>(0.3));
    pathsData.push_back(static_cast<float>(0.0));
    pathsData.push_back(static_cast<float>(0));
    pathsData.push_back(static_cast<float>(0.3));
    pathsData.push_back(static_cast<float>(0));
    pathsData.push_back(static_cast<float>(1));
    m_paths_meta.clear();

    for (const auto &path : paths) {
        auto path_points = path.getPointsPath();
        size_t ptr = pathsData.size() / 2;
        for (const auto &v : path_points) {
            pathsData.push_back(static_cast<float>(v.x()));
            pathsData.push_back(static_cast<float>(v.y()));
        }
        m_paths_meta.emplace_back(ptr, &path, path_points.size(),
                                  path.getType());
    }

    vbo->bind();
    vbo->allocate(pathsData.data(), (int)(sizeof(GLfloat) * pathsData.size()));
    vbo->release();
}

void GlPaths::setAppearenceSettings(const AppearenceSettings& settings)
{
    appearance_settings = settings;
}

void GlPaths::showManeuvers(bool should_show)
{
    show_maneuvers = should_show;
}

void GlPaths::render() const
{
    m_program->bind();
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw paths
    vbo->bind();
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 0.0f);
    glVertexAttrib1f(2, 0.0f);
    glVertexAttrib1f(4, 1.0f); // scale

    for (const auto &path_meta : m_paths_meta) {
        if (path_meta.path->isManeuver() && !show_maneuvers) {
            continue;
        }
        
        const auto &color = appearance_settings.path_colors[static_cast<size_t>(path_meta.path->getType())];
        glVertexAttrib3f(3, color.x, color.y, color.z);
        glDrawArrays(GL_LINE_STRIP, static_cast<GLint>(path_meta.ptr),
                    static_cast<GLsizei>(path_meta.points_count));
    }

    
    // Paths start points
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttrib1f(4, 0.05f); // scale

    for (const auto &path_meta : m_paths_meta) {
        if (path_meta.path->isManeuver() && !show_maneuvers) {
            continue;
        }

        const auto &color =
            appearance_settings.path_colors[static_cast<size_t>(path_meta.path->getType())];
        glVertexAttrib3f(3, color.x, color.y, color.z);

        for (const auto &segment : path_meta.path->getSegments()) {
            const auto start_point = segment.second.getStartPoint();
            glVertexAttrib2f(1, static_cast<GLfloat>(start_point.x()),
                            static_cast<GLfloat>(start_point.y()));
            glVertexAttrib1f(
                2, static_cast<GLfloat>(segment.second.getBeginAngle().radians()));
            glDrawArrays(GL_LINE_LOOP, 0, PATH_POINT_MARK_N);
        }
    }

    vbo->release();
    m_program->release();
}
