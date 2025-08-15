#include "../include/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace Engine {

// Static global logger instance
static std::unique_ptr<Logger> globalLogger = nullptr;

// Logger implementation
Logger::Logger(std::unique_ptr<ILogOutput> output, LogLevel level)
    : minLevel(level), output(std::move(output)), enabled(true) {
}

void Logger::log(LogLevel level, const std::string& category, const std::string& message) {
    if (!enabled || !shouldLog(level) || !output) {
        return;
    }
    
    output->write(level, category, message);
}

void Logger::debug(const std::string& category, const std::string& message) {
    log(LogLevel::DEBUG, category, message);
}

void Logger::info(const std::string& category, const std::string& message) {
    log(LogLevel::INFO, category, message);
}

void Logger::warn(const std::string& category, const std::string& message) {
    log(LogLevel::WARN, category, message);
}

void Logger::error(const std::string& category, const std::string& message) {
    log(LogLevel::ERROR, category, message);
}

bool Logger::shouldLog(LogLevel level) const {
    return enabled && level >= minLevel;
}

void Logger::setMinLevel(LogLevel level) {
    minLevel = level;
}

LogLevel Logger::getMinLevel() const {
    return minLevel;
}

void Logger::setEnabled(bool enable) {
    enabled = enable;
}

bool Logger::isEnabled() const {
    return enabled;
}

void Logger::flush() {
    if (output) {
        output->flush();
    }
}

// ConsoleOutput implementation
void ConsoleOutput::write(LogLevel level, const std::string& category, const std::string& message) {
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // Format timestamp
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    timestamp << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    // Convert log level to string
    const char* levelStr;
    switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO:  levelStr = "INFO "; break;
        case LogLevel::WARN:  levelStr = "WARN "; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        default:              levelStr = "UNKNOWN"; break;
    }
    
    // Output formatted message to console
    std::cout << "[" << timestamp.str() << "] [" << levelStr << "] [" 
              << category << "] " << message << std::endl;
}

void ConsoleOutput::flush() {
    std::cout.flush();
}

// FileOutput implementation
FileOutput::FileOutput(const std::string& filename, bool append) 
    : filename(filename) {
    auto mode = append ? (std::ios::out | std::ios::app) : std::ios::out;
    file.open(filename, mode);
}

FileOutput::~FileOutput() {
    if (file.is_open()) {
        file.close();
    }
}

void FileOutput::write(LogLevel level, const std::string& category, const std::string& message) {
    if (!file.is_open()) {
        return;
    }
    
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // Format timestamp
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    timestamp << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    // Convert log level to string
    const char* levelStr;
    switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO:  levelStr = "INFO "; break;
        case LogLevel::WARN:  levelStr = "WARN "; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        default:              levelStr = "UNKNOWN"; break;
    }
    
    // Write formatted message to file
    file << "[" << timestamp.str() << "] [" << levelStr << "] [" 
         << category << "] " << message << std::endl;
}

void FileOutput::flush() {
    if (file.is_open()) {
        file.flush();
    }
}

bool FileOutput::isOpen() const {
    return file.is_open();
}

// GlobalLogger implementation
namespace GlobalLogger {
    void setLogger(std::unique_ptr<Logger> logger) {
        globalLogger = std::move(logger);
    }
    
    Logger* getLogger() {
        return globalLogger.get();
    }
    
    bool hasLogger() {
        return globalLogger != nullptr;
    }
}

} // namespace Engine