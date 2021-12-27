#ifndef GLPATHS
#define GLPATHS

#include "usvdata/Path.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>


class Program;
class Buffer;

class GLPaths
{
public:
    struct AppearenceSettings {
        glm::vec4 path_colors[static_cast<size_t>(USV::PathType::End)];
    } appearance_settings;

    void setAppearenceSettings(const AppearenceSettings& settings);
    void initVbo(const std::vector<USV::Path>& paths);
    void showWastedManeuvers(bool should_show);
    void render() const;

    GLPaths();
private:
    static const std::string fragmentShaderSource;
    static const std::string vertexShaderSource;

    struct pathVBOMeta {
        size_t ptr;
        const USV::Path *path;
        size_t points_count;

        pathVBOMeta(size_t ptr, const USV::Path *path, size_t points_count)
            : ptr(ptr), path(path), points_count(points_count){};
    };
private:
    std::unique_ptr<Program> m_program;
    std::unique_ptr<Buffer> vbo;
    std::vector<pathVBOMeta> m_paths_meta;
    bool show_wasted_maneuvers{true};
};

#endif // GLPATHS
