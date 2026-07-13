#ifndef VIEWPORT_LAYOUT_HPP
#define VIEWPORT_LAYOUT_HPP

#include <glm/glm.hpp>

struct ViewportLayout {
    int render_width = 0;
    int render_height = 0;
    bool visible = false;
    bool hovered = false;
};

struct ViewportClientBounds {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    [[nodiscard]] bool is_valid() const noexcept {
        return this->width >= 1.0f && this->height >= 1.0f;
    }

    [[nodiscard]] bool contains(const float mouse_x, const float mouse_y) const noexcept {
        return mouse_x >= this->x
            && mouse_x < this->x + this->width
            && mouse_y >= this->y
            && mouse_y < this->y + this->height;
    }
};

inline glm::vec2 viewport_client_to_framebuffer(
    const glm::vec2& client_mouse,
    const ViewportClientBounds& bounds,
    const glm::vec2& framebuffer_size
) {
    if (!bounds.is_valid() || !bounds.contains(client_mouse.x, client_mouse.y)) {
        return glm::vec2(-1.0f);
    }

    const float local_x = client_mouse.x - bounds.x;
    const float local_y = client_mouse.y - bounds.y;

    return glm::vec2(
        local_x * framebuffer_size.x / bounds.width,
        local_y * framebuffer_size.y / bounds.height
    );
}

#endif
