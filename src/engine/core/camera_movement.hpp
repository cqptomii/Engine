/**
 * @file camera_movement.hpp
 * @author Tom FRAISSE
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #ifndef CAMERA_MOVEMENT_HPP
 #define CAMERA_MOVEMENT_HPP

/**
 * @brief Enum to manage the camera movement
 * @details Enum to manage the camera movement in the editor
 * @enum CameraMovement
 * @value FORWARD Forward movement
 * @value BACKWARD Backward movement
 * @value LEFT Left movement
 * @value RIGHT Right movement
 */
 enum CameraMovement
 {
     FORWARD = 1,
     BACKWARD = 2,
     LEFT = 3,
     RIGHT = 4,
     TOP = 5,
     BOTTOM = 6
 };

 #endif // CAMERA_MOVEMENT_HPP