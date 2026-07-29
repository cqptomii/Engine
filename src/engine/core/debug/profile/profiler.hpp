
/**
 * @file profiler.hpp
 * @author FRAISSE Tom
 * @brief Profiler class for the engine debug system
 * @details This class is responsible for the profiling of the engine debug system
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_PROFILER_HPP
#define ENGINE_DEBUG_PROFILER_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

class Profiler {
    // The clock type
    using Clock = std::chrono::steady_clock;

    // The map of the scope names and the elapsed time in milliseconds
    std::unordered_map<std::string, double> scope_ms;

    // The total elapsed time in milliseconds for the current frame
    double frame_total_ms = 0.0;

public:

    /**
     * @brief Method to begin a frame
     * 
     * @details It clears the scope map and resets the frame total
     * @return void
     */
    void begin_frame() {
        this->scope_ms.clear();
        this->frame_total_ms = 0.0;
    }

    /**
     * @brief Method to add a scope sample into the profiler
     * 
     * @param scope_name The name of the scope
     * @param elapsed_ms The elapsed time in milliseconds for the scope
     * @return void
     */
    void add_scope_sample(const std::string& scope_name, const double elapsed_ms) {
        this->scope_ms[scope_name] += elapsed_ms;
        this->frame_total_ms += elapsed_ms;
    }

    /**
     * @brief Method to print the profiler
     * 
     * @details This method is used to print the profiler
     * @details If the scope map is empty, it will return
     * @details If the scope map is not empty, it will print the scope names and the elapsed time in milliseconds
     * @return void
     */
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

    /**
     * @brief Method to get the scopes of the profiler
     * 
     * @return const std::unordered_map<std::string, double>& The scopes of the profiler
     */
    const std::unordered_map<std::string, double>& scopes() const {
        return this->scope_ms;
    }

    /**
     * @brief Method to get the total elapsed time in milliseconds for the current frame
     * 
     * @return double The total elapsed time in milliseconds for the current frame
     */
    double frame_total() const {
        return this->frame_total_ms;
    }
};




/**
 * @brief ScopedTimer class for the engine debug system
 * @details This class is responsible for the scoped timing of the engine debug system
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
class ScopedTimer {
    // The clock type
    using Clock = std::chrono::steady_clock;

    // The profiler reference
    Profiler& profiler;

    // The name of the scope
    std::string scope_name;

    // The start time of the scope
    Clock::time_point start_time;

public:

    /**
     * @brief Constructor of the ScopedTimer class
     * 
     * @param profiler_ref The profiler reference
     * @param name The name of the scope
     * @return void
     */
    ScopedTimer(Profiler& profiler_ref, std::string name): profiler(profiler_ref), scope_name(std::move(name)), start_time(Clock::now()) {}

    /**
     * @brief Destroy the Scoped Timer object
     * 
     * @details This method is used to destroy the ScopedTimer object
     * @details It adds the elapsed time to the profiler
     * @return void
     */
    ~ScopedTimer() {
        const auto end_time = Clock::now();

        // Calculate the elapsed time in milliseconds
        const double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - this->start_time).count();
        
        // Add the elapsed time to the profiler
        this->profiler.add_scope_sample(this->scope_name, elapsed_ms);
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
};

#endif
