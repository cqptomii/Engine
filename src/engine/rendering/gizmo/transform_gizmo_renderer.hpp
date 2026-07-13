#ifndef TRANSFORM_GIZMO_RENDERER_HPP
#define TRANSFORM_GIZMO_RENDERER_HPP

#include <cmath>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "engine/core/utils.hpp"
#include "engine/core/debug/debug_hooks.hpp"
#include "engine/editor/picking/manipulation_mode.hpp"
#include "engine/editor/picking/selection_context.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/rendering/utils/transform_data.hpp"
#include "engine/resources/cpu/shader_resource.hpp"
#include "engine/resources/gpu/shader.hpp"
#include "engine/resources/gpu/wrappers/Buffer.hpp"
#include "engine/resources/gpu/wrappers/VAO.hpp"

/**
 * @brief Renders the transform gizmo (translate / rotate / scale) in world
 *        space at the selection pivot.
 *
 * Distinct from GizmoRenderer (the screen-corner orientation axes). This gizmo
 * lives in the scene: it is placed at the selection pivot, oriented with the
 * selection frame, and scaled with the camera distance so it keeps a constant
 * on-screen size. It reuses the editor_axes shader (position + color, SSBO with
 * one instance).
 */
class TransformGizmoRenderer
{
    // Fraction of the camera-to-pivot distance used as the gizmo world size.
    static constexpr float k_gizmo_screen_scale = 0.15f;
    static constexpr int k_ring_segments = 48;

    VAO translate_vao;
    Buffer translate_vbo;
    GLsizei translate_vertex_count = 0;

    VAO rotate_vao;
    Buffer rotate_vbo;
    GLsizei rotate_ring_vertex_count = 0;

    VAO scale_vao;
    Buffer scale_vbo;
    GLsizei scale_vertex_count = 0;

    Buffer gizmo_ssbo;

    std::string axes_vs_path;
    std::string axes_fs_path;

    static void push_vertex(std::vector<float>& out, const glm::vec3& pos, const glm::vec3& color)
    {
        out.push_back(pos.x);
        out.push_back(pos.y);
        out.push_back(pos.z);
        out.push_back(color.x);
        out.push_back(color.y);
        out.push_back(color.z);
    }

    static void push_line(std::vector<float>& out, const glm::vec3& a, const glm::vec3& b, const glm::vec3& color)
    {
        push_vertex(out, a, color);
        push_vertex(out, b, color);
    }

    // 12 edges of an axis-aligned wire cube centered on 'center' with half extent 'half'.
    static void push_wire_cube(std::vector<float>& out, const glm::vec3& center, float half, const glm::vec3& color)
    {
        const glm::vec3 corners[8] = {
            center + glm::vec3(-half, -half, -half),
            center + glm::vec3( half, -half, -half),
            center + glm::vec3( half,  half, -half),
            center + glm::vec3(-half,  half, -half),
            center + glm::vec3(-half, -half,  half),
            center + glm::vec3( half, -half,  half),
            center + glm::vec3( half,  half,  half),
            center + glm::vec3(-half,  half,  half)
        };

        const int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };

        for (const auto& edge : edges)
        {
            push_line(out, corners[edge[0]], corners[edge[1]], color);
        }
    }

    void upload(VAO& vao, Buffer& vbo, const std::vector<float>& vertices)
    {
        vao.bind();
        vbo.bind();
        vbo.set_data(vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        vao.set_vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
        vao.set_vertex_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        vao.unbind();
        vbo.unbind();
    }

    void build_translate_geometry()
    {
        const glm::vec3 red(1.0f, 0.0f, 0.0f);
        const glm::vec3 green(0.0f, 1.0f, 0.0f);
        const glm::vec3 blue(0.0f, 0.0f, 1.0f);

        std::vector<float> vertices;
        push_line(vertices, glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), red);
        push_line(vertices, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), green);
        push_line(vertices, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), blue);

        translate_vertex_count = static_cast<GLsizei>(vertices.size() / 6);
        upload(translate_vao, translate_vbo, vertices);
    }

    void build_rotate_geometry()
    {
        const glm::vec3 red(1.0f, 0.0f, 0.0f);
        const glm::vec3 green(0.0f, 1.0f, 0.0f);
        const glm::vec3 blue(0.0f, 0.0f, 1.0f);

        std::vector<float> vertices;
        // Three rings drawn back to back as GL_LINE_LOOP segments.
        for (int i = 0; i < k_ring_segments; ++i)
        {
            const float angle = (2.0f * glm::pi<float>() * static_cast<float>(i)) / static_cast<float>(k_ring_segments);
            const float c = std::cos(angle);
            const float s = std::sin(angle);
            push_vertex(vertices, glm::vec3(0.0f, c, s), red);   // ring around X (plane YZ)
        }
        for (int i = 0; i < k_ring_segments; ++i)
        {
            const float angle = (2.0f * glm::pi<float>() * static_cast<float>(i)) / static_cast<float>(k_ring_segments);
            const float c = std::cos(angle);
            const float s = std::sin(angle);
            push_vertex(vertices, glm::vec3(c, 0.0f, s), green); // ring around Y (plane XZ)
        }
        for (int i = 0; i < k_ring_segments; ++i)
        {
            const float angle = (2.0f * glm::pi<float>() * static_cast<float>(i)) / static_cast<float>(k_ring_segments);
            const float c = std::cos(angle);
            const float s = std::sin(angle);
            push_vertex(vertices, glm::vec3(c, s, 0.0f), blue);  // ring around Z (plane XY)
        }

        rotate_ring_vertex_count = static_cast<GLsizei>(k_ring_segments);
        upload(rotate_vao, rotate_vbo, vertices);
    }

    void build_scale_geometry()
    {
        const glm::vec3 red(1.0f, 0.0f, 0.0f);
        const glm::vec3 green(0.0f, 1.0f, 0.0f);
        const glm::vec3 blue(0.0f, 0.0f, 1.0f);
        const float box_half = 0.08f;

        std::vector<float> vertices;
        push_line(vertices, glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), red);
        push_line(vertices, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), green);
        push_line(vertices, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), blue);

        push_wire_cube(vertices, glm::vec3(1.0f, 0.0f, 0.0f), box_half, red);
        push_wire_cube(vertices, glm::vec3(0.0f, 1.0f, 0.0f), box_half, green);
        push_wire_cube(vertices, glm::vec3(0.0f, 0.0f, 1.0f), box_half, blue);

        scale_vertex_count = static_cast<GLsizei>(vertices.size() / 6);
        upload(scale_vao, scale_vbo, vertices);
    }

    glm::mat4 build_model_matrix(const SelectionContext& ctx, const CameraData& camera) const
    {
        float distance = glm::length(camera.position - ctx.pivot_world);
        distance = glm::max(distance, 0.001f);
        const float gizmo_size = distance * k_gizmo_screen_scale;

        return glm::translate(glm::mat4(1.0f), ctx.pivot_world)
             * glm::mat4_cast(ctx.orientation)
             * glm::scale(glm::mat4(1.0f), glm::vec3(gizmo_size));
    }

public:
    TransformGizmoRenderer()
        : translate_vao(), translate_vbo(GL_ARRAY_BUFFER),
          rotate_vao(), rotate_vbo(GL_ARRAY_BUFFER),
          scale_vao(), scale_vbo(GL_ARRAY_BUFFER),
          gizmo_ssbo(GL_SHADER_STORAGE_BUFFER)
    {
        build_translate_geometry();
        build_rotate_geometry();
        build_scale_geometry();

        axes_vs_path = resolve_shader_path("assets/shaders/editor/editor_axes.vs");
        axes_fs_path = resolve_shader_path("assets/shaders/editor/editor_axes.fs");
    }

    ~TransformGizmoRenderer() = default;

    TransformGizmoRenderer(const TransformGizmoRenderer&) = delete;
    TransformGizmoRenderer& operator=(const TransformGizmoRenderer&) = delete;

    void render(ManipulationMode mode, const SelectionContext& ctx, const CameraData& camera)
    {
        if (mode == ManipulationMode::NONE || !ctx.valid)
        {
            return;
        }

        static ShaderResource axes_shader_resource(axes_vs_path.c_str(), axes_fs_path.c_str());
        static Shader axes_shader(
            axes_shader_resource.get_vertex_source().c_str(),
            axes_shader_resource.get_fragment_source().c_str()
        );

        // Save the OpenGL state we mutate so the scene rendering is untouched.
        const GLboolean was_depth_test = glIsEnabled(GL_DEPTH_TEST);
        const GLboolean was_cull_face = glIsEnabled(GL_CULL_FACE);
        const GLboolean was_blend = glIsEnabled(GL_BLEND);
        GLfloat previous_line_width = 1.0f;
        glGetFloatv(GL_LINE_WIDTH, &previous_line_width);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(2.5f);

        // One instance: the gizmo transform placed at the selection pivot.
        const TransformData transform{
            build_model_matrix(ctx, camera),
            camera.view,
            camera.projection
        };
        gizmo_ssbo.bind();
        gizmo_ssbo.set_data(sizeof(TransformData), &transform, GL_DYNAMIC_DRAW);
        gizmo_ssbo.unbind();

        axes_shader.use();
        axes_shader.set_float("uAlpha", 1.0f);
        gizmo_ssbo.bind_base(0);

        switch (mode)
        {
            case ManipulationMode::TRANSLATE:
                translate_vao.bind();
                glDrawArrays(GL_LINES, 0, translate_vertex_count);
                debug_record_raw_draw_call();
                translate_vao.unbind();
                break;

            case ManipulationMode::ROTATE:
                rotate_vao.bind();
                // Three separate rings, each as its own closed loop.
                glDrawArrays(GL_LINE_LOOP, 0, rotate_ring_vertex_count);
                glDrawArrays(GL_LINE_LOOP, rotate_ring_vertex_count, rotate_ring_vertex_count);
                glDrawArrays(GL_LINE_LOOP, rotate_ring_vertex_count * 2, rotate_ring_vertex_count);
                debug_record_raw_draw_call();
                debug_record_raw_draw_call();
                debug_record_raw_draw_call();
                rotate_vao.unbind();
                break;

            case ManipulationMode::SCALE_UNIFORM:
            case ManipulationMode::SCALE_NON_UNIFORM:
                scale_vao.bind();
                glDrawArrays(GL_LINES, 0, scale_vertex_count);
                debug_record_raw_draw_call();
                scale_vao.unbind();
                break;

            case ManipulationMode::NONE:
            default:
                break;
        }

        Shader::unuse();

        glLineWidth(previous_line_width);
        if (was_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (was_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (was_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    }
};

#endif // TRANSFORM_GIZMO_RENDERER_HPP
