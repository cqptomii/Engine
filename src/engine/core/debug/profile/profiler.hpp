#ifndef ENGINE_DEBUG_PROFILER_HPP
#define ENGINE_DEBUG_PROFILER_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

class Profiler {
    using Clock = std::chrono::steady_clock;

    std::unordered_map<std::string, double> scope_ms;
    double frame_total_ms = 0.0;

public:
    void begin_frame() {
        this->scope_ms.clear();
        this->frame_total_ms = 0.0;
    }

    void add_scope_sample(const std::string& scope_name, const double elapsed_ms) {
        this->scope_ms[scope_name] += elapsed_ms;
        this->frame_total_ms += elapsed_ms;
    }

    void print() const {
        if (this->scope_ms.empty()) {
            return;
        }

        std::cout << "Profiler (last frame ms):" << std::endl;
        for (const auto& [scope_name, elapsed_ms] : this->scope_ms) {
            std::cout << "  - " << scope_name << ": " << elapsed_ms << " ms" << std::endl;
        }
        std::cout << "  total (scoped): " << this->frame_total_ms << " ms" << std::endl;
    }

    const std::unordered_map<std::string, double>& scopes() const {
        return this->scope_ms;
    }

    double frame_total() const {
        return this->frame_total_ms;
    }
};

class ScopedTimer {
    using Clock = std::chrono::steady_clock;

    Profiler& profiler;
    std::string scope_name;
    Clock::time_point start_time;

public:
    ScopedTimer(Profiler& profiler_ref, std::string name)
        : profiler(profiler_ref)
        , scope_name(std::move(name))
        , start_time(Clock::now()) {}

    ~ScopedTimer() {
        const auto end_time = Clock::now();
        const double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - this->start_time).count();
        this->profiler.add_scope_sample(this->scope_name, elapsed_ms);
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

#endif
