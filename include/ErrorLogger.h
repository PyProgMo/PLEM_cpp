#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <vector>
#include <filesystem>
#include "ErrorCodes.h"

namespace fs = std::filesystem;

class ErrorLogger {
private:
    static ErrorLogger* instance;
    static std::mutex instanceMutex;
    
    std::string logDirectory;
    std::string currentLogFile;
    mutable std::mutex logMutex;
    bool loggingEnabled;
    std::vector<std::pair<std::string, std::string>> errorHistory; // timestamp, message

    // Private constructor
    ErrorLogger() : loggingEnabled(true) {
        logDirectory = "log/errors";
        InitializeLogDirectory();
    }

    // Initialize log directory
    void InitializeLogDirectory() {
        try {
            if (!fs::exists(logDirectory)) {
                fs::create_directories(logDirectory);
            }
        } catch (const std::exception& e) {
            // Fallback if directory creation fails
            logDirectory = "./log/errors";
        }
    }

    // Generate timestamp string
    std::string GetTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%d.%m.%Y %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    // Generate date string for log filename
    std::string GetDateString() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d");
        return ss.str();
    }

    // Get error category name
    std::string GetCategoryName(ErrorCodes::ErrorCategory category) const {
        switch (category) {
            case ErrorCodes::CATEGORY_SYSTEM:
                return "SYSTEM";
            case ErrorCodes::CATEGORY_FRONTEND:
                return "FRONTEND";
            case ErrorCodes::CATEGORY_BACKEND:
                return "BACKEND";
            case ErrorCodes::CATEGORY_FBCONNECTOR:
                return "FB_CONNECTOR";
            case ErrorCodes::CATEGORY_CAMERA:
                return "CAMERA";
            case ErrorCodes::CATEGORY_LASER:
                return "LASER";
            case ErrorCodes::CATEGORY_SPECTROGRAPH:
                return "SPECTROGRAPH";
            case ErrorCodes::CATEGORY_POWERMETER:
                return "POWERMETER";
            case ErrorCodes::CATEGORY_STAGE:
                return "STAGE";
            case ErrorCodes::CATEGORY_FILE_IO:
                return "FILE_IO";
            case ErrorCodes::CATEGORY_CONFIG:
                return "CONFIG";
            case ErrorCodes::CATEGORY_MEASUREMENT:
                return "MEASUREMENT";
            default:
                return "UNKNOWN";
        }
    }

    // Format error code as hex string
    std::string FormatErrorCode(uint32_t errorCode) const {
        std::stringstream ss;
        ss << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(6)
           << errorCode;
        return ss.str();
    }

    // Create log entry string
    std::string CreateLogEntry(uint32_t errorCode, const std::string& errorMessage,
                               const std::string& details = "") const {
        std::stringstream ss;
        auto timestamp = GetTimestamp();
        auto category = ErrorCodes::GetCategory(errorCode);

        ss << "[" << timestamp << "] ";
        ss << "[" << GetCategoryName(category) << "] ";
        ss << "[" << FormatErrorCode(errorCode) << "] ";
        ss << errorMessage;

        if (!details.empty()) {
            ss << " | Details: " << details;
        }

        return ss.str();
    }

    // Write to log file
    void WriteToFile(const std::string& logEntry) {
        try {
            std::string dateStr = GetDateString();
            std::string logFilePath = logDirectory + "/errors_" + dateStr + ".log";

            std::ofstream logFile(logFilePath, std::ios::app);
            if (logFile.is_open()) {
                logFile << logEntry << "\n";
                logFile.flush();
                logFile.close();
                currentLogFile = logFilePath;
            }
        } catch (const std::exception& e) {
            // Silent fail for file operations
        }
    }

public:
    // Delete copy constructor and assignment operator
    ErrorLogger(const ErrorLogger&) = delete;
    ErrorLogger& operator=(const ErrorLogger&) = delete;

    // Get singleton instance
    static ErrorLogger& GetInstance() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (instance == nullptr) {
            instance = new ErrorLogger();
        }
        return *instance;
    }

    // Log error with code, message, and optional details
    void LogError(uint32_t errorCode, const std::string& errorMessage,
                  const std::string& details = "") {
        std::lock_guard<std::mutex> lock(logMutex);

        std::string logEntry = CreateLogEntry(errorCode, errorMessage, details);

        if (loggingEnabled) {
            WriteToFile(logEntry);
        }

        // Store in history (keep last 1000 errors)
        errorHistory.push_back({GetTimestamp(), errorMessage});
        if (errorHistory.size() > 1000) {
            errorHistory.erase(errorHistory.begin());
        }
    }

    // Log error by category and specific error code (convenience method)
    void LogError(ErrorCodes::ErrorCategory category, uint16_t specificError,
                  const std::string& errorMessage, const std::string& details = "") {
        uint32_t errorCode = ErrorCodes::CreateErrorCode(category, specificError);
        LogError(errorCode, errorMessage, details);
    }

    // Get the current log file path
    std::string GetCurrentLogFile() const {
        std::lock_guard<std::mutex> lock(logMutex);
        return currentLogFile;
    }

    // Get the log directory
    std::string GetLogDirectory() const {
        return logDirectory;
    }

    // Set logging enabled/disabled
    void SetLoggingEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(logMutex);
        loggingEnabled = enabled;
    }

    // Check if logging is enabled
    bool IsLoggingEnabled() const {
        std::lock_guard<std::mutex> lock(logMutex);
        return loggingEnabled;
    }

    // Get error history (last N errors)
    std::vector<std::pair<std::string, std::string>> GetErrorHistory(size_t count = 50) const {
        std::lock_guard<std::mutex> lock(logMutex);
        size_t start = errorHistory.size() > count ? errorHistory.size() - count : 0;
        return std::vector<std::pair<std::string, std::string>>(
            errorHistory.begin() + start, errorHistory.end());
    }

    // Get last error
    std::pair<std::string, std::string> GetLastError() const {
        std::lock_guard<std::mutex> lock(logMutex);
        if (!errorHistory.empty()) {
            return errorHistory.back();
        }
        return {"", "No errors logged"};
    }

    // Clear error history
    void ClearErrorHistory() {
        std::lock_guard<std::mutex> lock(logMutex);
        errorHistory.clear();
    }

    // Get all log files in directory
    std::vector<std::string> GetAllLogFiles() const {
        std::vector<std::string> logFiles;
        try {
            if (fs::exists(logDirectory)) {
                for (const auto& entry : fs::directory_iterator(logDirectory)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".log") {
                        logFiles.push_back(entry.path().filename().string());
                    }
                }
            }
        } catch (const std::exception& e) {
            // Silent fail
        }
        return logFiles;
    }
};

// Initialize static members
ErrorLogger* ErrorLogger::instance = nullptr;
std::mutex ErrorLogger::instanceMutex;

// Convenience macros for logging
#define LOG_ERROR(code, msg) ErrorLogger::GetInstance().LogError(code, msg)
#define LOG_ERROR_DETAILS(code, msg, details) ErrorLogger::GetInstance().LogError(code, msg, details)
#define LOG_ERROR_CAT(category, code, msg) \
    ErrorLogger::GetInstance().LogError(category, code, msg)
#define LOG_ERROR_CAT_DETAILS(category, code, msg, details) \
    ErrorLogger::GetInstance().LogError(category, code, msg, details)
