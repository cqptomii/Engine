#ifndef ENGINE_DEBUG_LOG_TYPES_HPP
#define ENGINE_DEBUG_LOG_TYPES_HPP

#include <string>

enum class LogLevel {
    Trace,
    Info,
    Warning,
    Error
};

struct LogEntry {
    LogLevel level = LogLevel::Info;
    std::string category;
    std::string message;
};

#endif
