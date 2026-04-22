//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_CAMERA_DATA_HPP
#define ENGINE_CAMERA_DATA_HPP

#include <glm/glm.hpp>

struct CameraData
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;
};

#endif //ENGINE_CAMERA_DATA_HPP