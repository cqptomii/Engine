/**
 * @file frame_stats.hpp
 * @author FRAISSE Tom
 * @brief Frame stats class for the engine debug system
 * @details This class is responsible for the tracking of the frame stats for the engine debug system
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_FRAME_STATS_HPP
#define ENGINE_DEBUG_FRAME_STATS_HPP

class FrameStats {
    // The delta time
    float delta_time = 0.0f;

    // The frame time in milliseconds
    float frame_ms = 0.0f;

public:

    /**
     * @brief Method to handle the frame
     * 
     * @param delta The delta time
     * @return void
     */
    void on_frame(const float delta) {
        // Update the delta time
        this->delta_time = delta;
        this->frame_ms = delta * 1000.0f;
    }
    
    /**
     * @brief Method to get the delta time
     * 
     * @return float The delta time
     */
    float get_delta_time() const {
        return this->delta_time;
    }

    /**
     * @brief Method to get the frame time in milliseconds
     * 
     * @return float The frame time in milliseconds
     */
    float get_frame_ms() const {
        return this->frame_ms;
    }
};

#endif
