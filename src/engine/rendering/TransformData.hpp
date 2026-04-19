#ifndef ENGINE_TRANSFORM_DATA_HPP
#define ENGINE_TRANSFORM_DATA_HPP

#include <glm/glm.hpp>

struct TransformData{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

#endif // ENGINE_TRANSFORM_DATA_HPP