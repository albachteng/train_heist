#pragma once

#include <string>
#include <fstream>
#include <memory>

namespace Engine {

/**
 * LogLevel - Defines logging severity levels
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

/**
 * ILogOutput - Interface for log output destinations
 * Allows dependency injection of different output targets (console, file, etc.)
 */
class ILogOutput {
public:
    virtual ~ILogOutput() = default;
    virtual void write(LogLevel level, const std::string& category, const std::string& message) = 0;
    virtual void flush() = 0;
};

/**
 * Logger - Central logging system for the engine
 * 
 * Features:
 * - Multiple log levels with runtime filtering
 * - Category-based logging for different systems
 * - Multiple output destinations via dependency injection
 * - Thread-safe design (prepared for future multi-threading)
 * - Minimal performance overhead with compile-time filtering
 */
class Logger {
private:
    LogLevel minLevel;
    std::unique_ptr<ILogOutput> output;
    bool enabled;

public:
    /**
     * Construct logger with output destination and minimum level
     * @param output Unique pointer to output implementation
     * @param level Minimum log level to output
     */
    explicit Logger(std::unique_ptr<ILogOutput> output, LogLevel level = LogLevel::INFO);
    
    ~Logger() = default;
    
    // Non-copyable but movable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;
    
    /**
     * Log a message at the specified level
     * @param level Log level for this message
     * @param category System/component category (e.g., "EntityManager", "Physics")
     * @param message Log message content
     */
    void log(LogLevel level, const std::string& category, const std::string& message);
    
    /**
     * Convenience methods for each log level
     */
    void debug(const std::string& category, const std::string& message);
    void info(const std::string& category, const std::string& message);
    void warn(const std::string& category, const std::string& message);
    void error(const std::string& category, const std::string& message);
    
    /**
     * Check if a log level would be output (useful for expensive message construction)
     * @param level Log level to check
     * @return true if this level would be logged
     */
    bool shouldLog(LogLevel level) const;
    
    /**
     * Set minimum log level
     * @param level New minimum level
     */
    void setMinLevel(LogLevel level);
    
    /**
     * Get current minimum log level
     * @return Current minimum level
     */
    LogLevel getMinLevel() const;
    
    /**
     * Enable or disable logging entirely
     * @param enable true to enable, false to disable
     */
    void setEnabled(bool enable);
    
    /**
     * Check if logging is enabled
     * @return true if logging is enabled
     */
    bool isEnabled() const;
    
    /**
     * Flush any buffered output
     */
    void flush();
};

/**
 * ConsoleOutput - Logs to console/stdout
 */
class ConsoleOutput : public ILogOutput {
public:
    ConsoleOutput() = default;
    ~ConsoleOutput() = default;
    
    void write(LogLevel level, const std::string& category, const std::string& message) override;
    void flush() override;
};

/**
 * FileOutput - Logs to a file
 */
class FileOutput : public ILogOutput {
private:
    std::ofstream file;
    std::string filename;

public:
    /**
     * Create file output
     * @param filename Path to log file
     * @param append If true, append to existing file; if false, overwrite
     */
    explicit FileOutput(const std::string& filename, bool append = true);
    ~FileOutput();
    
    void write(LogLevel level, const std::string& category, const std::string& message) override;
    void flush() override;
    
    /**
     * Check if file is open and ready for writing
     * @return true if file is open
     */
    bool isOpen() const;
};

/**
 * Global logger instance for convenient access
 * Initialize with Logger::setGlobalLogger() before use
 */
namespace GlobalLogger {
    void setLogger(std::unique_ptr<Logger> logger);
    Logger* getLogger();
    bool hasLogger();
}

} // namespace Engine

/**
 * Convenience macros for global logging
 * Usage: LOG_INFO("EntityManager", "Created entity with ID: " + std::to_string(id));
 */
#define LOG_DEBUG(category, message) \
    do { if (Engine::GlobalLogger::hasLogger()) Engine::GlobalLogger::getLogger()->debug(category, message); } while(0)

#define LOG_INFO(category, message) \
    do { if (Engine::GlobalLogger::hasLogger()) Engine::GlobalLogger::getLogger()->info(category, message); } while(0)

#define LOG_WARN(category, message) \
    do { if (Engine::GlobalLogger::hasLogger()) Engine::GlobalLogger::getLogger()->warn(category, message); } while(0)

#define LOG_ERROR(category, message) \
    do { if (Engine::GlobalLogger::hasLogger()) Engine::GlobalLogger::getLogger()->error(category, message); } while(0)
