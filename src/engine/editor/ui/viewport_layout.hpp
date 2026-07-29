/**
 * @file viewport_layout.hpp
 * @author Tom FRAISSE
 * @brief Header file for the viewport layout
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef VIEWPORT_LAYOUT_HPP
#define VIEWPORT_LAYOUT_HPP

#include <glm/glm.hpp>

/**
 * @brief Struct representing the viewport layout
 *
 * @param render_width The width of the viewport
 * @param render_height The height of the viewport
 * @param visible Whether the viewport is visible
 * @param hovered Whether the viewport is hovered
 */
struct ViewportLayout {
    int render_width = 0;
    int render_height = 0;
    bool visible = false;
    bool hovered = false;
};

/**
 * @brief Struct representing the viewport client bounds
 *
 * @param x The x coordinate of the viewport client bounds
 * @param y The y coordinate of the viewport client bounds
 * @param width The width of the viewport client bounds
 * @param height The height of the viewport client bounds
 * 
 */
struct ViewportClientBounds {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    /**
     * @brief Check if the viewport client bounds is valid
     * 
     * @return true If the viewport client bounds is valid
     * @return false If the viewport client bounds is not valid
     */
    [[nodiscard]] bool is_valid() const noexcept {
        return this->width >= 1.0f && this->height >= 1.0f;
    }

    /**
     * @brief Check if the viewport client bounds contains a mouse position
     * 
     * @param mouse_x The x coordinate of the mouse position
     * @param mouse_y The y coordinate of the mouse position
     * @return true If the viewport client bounds contains the mouse position
     * @return false If the viewport client bounds does not contain the mouse position
     */
    [[nodiscard]] bool contains(const float mouse_x, const float mouse_y) const noexcept {
        return mouse_x >= this->x
            && mouse_x < this->x + this->width
            && mouse_y >= this->y
            && mouse_y < this->y + this->height;
    }
};

/**
 * @brief Convert a client mouse position to a framebuffer position
 * 
 * @param client_mouse The client mouse position
 * @param bounds The viewport client bounds
 * @param framebuffer_size The framebuffer size
 * @return glm::vec2 The framebuffer position
 */
inline glm::vec2 viewport_client_to_framebuffer(const glm::vec2& client_mouse, const ViewportClientBounds& bounds, const glm::vec2& framebuffer_size
) {

    // Check if the viewport client bounds is valid and contains the mouse position
    if (!bounds.is_valid() || !bounds.contains(client_mouse.x, client_mouse.y)) {
        return glm::vec2(-1.0f);
    }

    // Convert the client mouse position to a local position
    const float local_x = client_mouse.x - bounds.x;
    const float local_y = client_mouse.y - bounds.y;

    // Convert the local position to a framebuffer position
    return glm::vec2(
        local_x * framebuffer_size.x / bounds.width,
        local_y * framebuffer_size.y / bounds.height
    );
}

#endif
