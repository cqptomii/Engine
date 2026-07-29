/**
 * @file log_types.hpp
 * @author FRAISSE Tom
 * @brief Enumeration class for the log levels and the log entry structure 
 * @version 0.1
 * @date 2026-07-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ENGINE_DEBUG_LOG_TYPES_HPP
#define ENGINE_DEBUG_LOG_TYPES_HPP

#include <string>

/**
 * @brief Enumeration class for the log levels
 * 
 */
enum class LogLevel {
    Trace,
    Info,
    Warning,
    Error
};

/**
 * @brief Structure for the log entry
 * @param level The log type
 * @param category The category of the log
 * @param message The message of the log
 */
struct LogEntry {
    LogLevel level = LogLevel::Info;
    std::string category;
    std::string message;
};

#endif
