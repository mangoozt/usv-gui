#ifndef USV_GUI_DEFINES_H
#define USV_GUI_DEFINES_H

#define USV_GUI_MATRICES_BINDING 16
#define USV_GUI_LIGHTS_BINDING 17

#include <glm/glm.hpp>

struct Material{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess{256};
};

#endif //USV_GUI_DEFINES_H
