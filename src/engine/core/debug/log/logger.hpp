/**
 * @file logger.hpp
 * @author FRAISSE Tom
 * @brief Logger class for the engine debug system
 * @version 0.1
 * @date 2026-07-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_LOGGER_HPP
#define ENGINE_DEBUG_LOGGER_HPP

#include <iostream>
#include <string>
#include <string_view>

#include "engine/core/debug/log/log_buffer.hpp"

class Logger {
    // The minimum log level of each log
    LogLevel min_level = LogLevel::Info;

    // The buffer where the logs are pushed
    LogBuffer buffer;

    /**
     * @brief Convert the log level to a string
     * 
     * @param level The log level to convert
     * @return const char* 
     */
    static const char* level_to_string(const LogLevel level) {
        switch (level) {
            case LogLevel::Trace:
                return "Trace";
            case LogLevel::Info:
                return "Info";
            case LogLevel::Warning:
                return "Warning";
            case LogLevel::Error:
                return "Error";
            default:
                return "Unknown";
        }
    }

public:
    /**
     * @brief Set the minimum log level of each log
     * 
     * @param level The minimum log level
     */
    void set_min_level(const LogLevel level) {
        this->min_level = level;
    }

    /**
     * @brief Push a log entry to the buffer
     * 
     * @param level The log type
     * @param category The category of the log
     * @param message The message of the log
     * @return void
     */
    void log(const LogLevel level, const std::string_view category, const std::string_view message) {
        if (level < this->min_level) {
            return;
        }

        this->buffer.push(LogEntry{
            level,
            std::string(category),
            std::string(message)
        });

        std::ostream& out = (level >= LogLevel::Warning) ? std::cerr : std::cout;
        out << "[" << level_to_string(level) << "][" << category << "] " << message << std::endl;
    }

    /**
     * @brief Get the buffer where the logs are pushed
     * 
     * @return const LogBuffer& 
     */
    [[nodiscard]] const LogBuffer& get_buffer() const noexcept {
        return this->buffer;
    }

    /**
     * @brief Clear the buffer
     * 
     * @return void
     */
    void clear_buffer() {
        this->buffer.clear();
    }
};

#endif
