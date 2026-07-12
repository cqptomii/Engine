#ifndef ENGINE_DEBUG_LOGGER_HPP
#define ENGINE_DEBUG_LOGGER_HPP

#include <iostream>
#include <string_view>

enum class LogLevel {
    Trace,
    Info,
    Warning,
    Error
};

class Logger {
    LogLevel min_level = LogLevel::Info;

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
    void set_min_level(const LogLevel level) {
        this->min_level = level;
    }

    void log(const LogLevel level, const std::string_view category, const std::string_view message) {
        if (level < this->min_level) {
            return;
        }

        std::ostream& out = (level >= LogLevel::Warning) ? std::cerr : std::cout;
        out << "[" << level_to_string(level) << "][" << category << "] " << message << std::endl;
    }
};

#endif
