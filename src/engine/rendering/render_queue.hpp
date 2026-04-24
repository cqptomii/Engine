//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_RENDER_QUEUE_HPP
#define ENGINE_RENDER_QUEUE_HPP
#include <algorithm>
#include <vector>

#include "engine/resources/cpu/material/material_instance.hpp"
#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/resources/cpu/resource_handle.hpp"
struct RenderCommand
{
    ResourceHandle<MeshResource> mesh_handle{};
    MeshResource* mesh = nullptr;
    MaterialInstance* material = nullptr;
    glm::mat4 transform{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 projection{1.0f};
};

class RenderQueue
{
std::vector<RenderCommand> commands;

public:
    RenderQueue() = default;

    void push(const RenderCommand& command)
    {
        commands.push_back(command);
    }
    void clear()
    {
        commands.clear();
    }

    void sort()
    {
        std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b)
        {
            if (a.material != b.material)
                return a.material < b.material;

            return a.mesh < b.mesh;
        });
    }

    std::vector<RenderCommand>& get_commands()
    {
        return commands;
    }
};

#endif //ENGINE_RENDER_QUEUE_HPP