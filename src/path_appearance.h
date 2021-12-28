#ifndef PATH_APPEARENCE_H
#define PATH_APPEARENCE_H

#include "state_notifier.h"
#include "usvdata/Path.h"
#include <glm/glm.hpp>


struct PathAppearance
{
public:
    glm::vec4 path_colors[static_cast<size_t>(USV::PathType::End)];
    bool path_visability[static_cast<size_t>(USV::PathType::End)];
public:
    glm::vec4 getColor(const USV::Path& path) const;
    glm::vec4 getColor(const USV::PathType type) const;
    bool isVisible(const USV::Path& path) const;
    bool isVisible(USV::PathType type) const;
};

class PathAppearanceNotifier : public StateNotifier<PathAppearance>
{
public:
    PathAppearanceNotifier();
    void updateColor(USV::PathType type, const glm::vec4& color);
    void setVisible(USV::PathType type, bool visibility);
};

#endif // PATH_APPEARENCE_H