#ifndef ENGINE_DEBUG_FRAME_STATS_HPP
#define ENGINE_DEBUG_FRAME_STATS_HPP

class FrameStats {
    float delta_time = 0.0f;
    float frame_ms = 0.0f;

public:
    void on_frame(const float delta) {
        this->delta_time = delta;
        this->frame_ms = delta * 1000.0f;
    }

    float get_delta_time() const {
        return this->delta_time;
    }

    float get_frame_ms() const {
        return this->frame_ms;
    }
};

#endif
