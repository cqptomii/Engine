#ifndef UBO_TYPES_HPP
#define UBO_TYPES_HPP

#include <glm/glm.hpp>

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 projection;
};

struct ObjectUBO
{
    glm::mat4 model;
};
struct MaterialUBO
{
    glm::vec4 albedo;
    float metallic;
    float roughness;
};

#endif //UBO_TYPES_HPP