/**
 * @file manipulation_mode.hpp
 * @author FRAISSE Tom
 * @brief Enum class for the manipulation mode
 * @version 0.1
 * @date 2026-07-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MANIPULATION_MODE_HPP
#define MANIPULATION_MODE_HPP

/**
 * @brief Enum class for the manipulation mode
 * 
 * @enum ManipulationMode
 * @enum value NONE : No manipulation mode
 * @enum value TRANSLATE : Translation mode
 * @enum value ROTATE : Rotation mode
 * @enum value SCALE : Scaling mode
 */
enum class ManipulationMode{
    NONE = -1,
    TRANSLATE = 0,
    ROTATE = 1,
    SCALE_UNIFORM = 2,
    SCALE_NON_UNIFORM = 3
};

/**
 * @brief Struct for the pending manipulation
 * @param delta_x The delta x from the mouse movement
 * @param delta_y The delta y from the mouse movement
 * @param is_active Whether the manipulation is active
 */
 struct PendingManipulation{
    float delta_x = 0.0f;
    float delta_y = 0.0f;
    bool is_active = false;
    ManipulationMode mode = ManipulationMode::NONE;
};

inline const char* manipulation_mode_label(const ManipulationMode mode) {
    switch (mode) {
        case ManipulationMode::TRANSLATE:
            return "Translate (G)";
        case ManipulationMode::ROTATE:
            return "Rotate (R)";
        case ManipulationMode::SCALE_UNIFORM:
        case ManipulationMode::SCALE_NON_UNIFORM:
            return "Scale (S)";
        case ManipulationMode::NONE:
        default:
            return "None";
    }
}


#endif