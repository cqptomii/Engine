#ifndef SELECTION_CONTEXT_HPP
#define SELECTION_CONTEXT_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "entt/entity/fwd.hpp"

class Scene;

struct SelectionContext {
    glm::vec3 pivot_world{0.0f};
    glm::quat orientation{1.0f,0.0f,0.0f,0.0f};
    bool valid = false;
};


SelectionContext build_selection_context(Scene& scene, const std::vector<entt::entity>& entities);

#endif