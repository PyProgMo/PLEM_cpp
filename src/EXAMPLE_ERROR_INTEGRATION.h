// Example: Error Logging Integration for Devices
// This file demonstrates how to integrate error logging into device classes

#pragma once

#include "../include/ErrorLogger.h"
#include "../include/ErrorCodes.h"

// ============================================================================
// EXAMPLE 1: Camera Device Integration
// ============================================================================

class ExampleCameraDevice {
private:
    bool isConnected;
    int deviceId;

public:
    ExampleCameraDevice() : isConnected(false), deviceId(-1) {}

    bool Initialize(int cameraId) {
        deviceId = cameraId;
        try {
            // Simulate camera initialization
            if (!ConnectToCamera(cameraId)) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_CAMERA,
                    ErrorCodes::CameraErrors::CAMERA_INIT_FAILED,
                    "Failed to initialize camera device",
                    "Camera ID: " + std::to_string(cameraId) + 
                    ", Check USB connection and drivers"
                );
                return false;
            }

            // Simulate sensor initialization
            if (!InitializeSensor()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_CAMERA,
                    ErrorCodes::CameraErrors::CAMERA_INIT_FAILED,
                    "Failed to initialize camera sensor",
                    "Camera ID: " + std::to_string(cameraId)
                );
                return false;
            }

            isConnected = true;
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_CAMERA,
                ErrorCodes::CameraErrors::CAMERA_INIT_FAILED,
                std::string("Camera initialization exception: ") + e.what(),
                "Camera ID: " + std::to_string(cameraId)
            );
            return false;
        }
    }

    bool Capture(uint8_t* buffer, size_t bufferSize) {
        if (!isConnected) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_CAMERA,
                ErrorCodes::CameraErrors::CAMERA_DISCONNECTED,
                "Cannot capture: camera not connected",
                "Device ID: CAM_" + std::to_string(deviceId)
            );
            return false;
        }

        try {
            // Simulate capture operation with timeout
            if (!PerformCapture(buffer, bufferSize, 5000)) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_CAMERA,
                    ErrorCodes::CameraErrors::CAMERA_TIMEOUT,
                    "Camera capture timeout",
                    "Timeout: 5000ms, Buffer size: " + std::to_string(bufferSize)
                );
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_CAMERA,
                ErrorCodes::CameraErrors::CAMERA_CAPTURE_FAILED,
                std::string("Capture exception: ") + e.what()
            );
            return false;
        }
    }

private:
    bool ConnectToCamera(int id) {
        // Implementation
        return true;
    }

    bool InitializeSensor() {
        // Implementation
        return true;
    }

    bool PerformCapture(uint8_t* buffer, size_t size, int timeoutMs) {
        // Implementation
        return true;
    }
};

// ============================================================================
// EXAMPLE 2: Laser Device Integration
// ============================================================================

class ExampleLaserDevice {
private:
    bool isEnabled;
    float currentPower;
    float currentTemp;

public:
    ExampleLaserDevice() 
        : isEnabled(false), currentPower(0.0f), currentTemp(25.0f) {}

    bool Initialize() {
        try {
            if (!ConnectLaser()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_LASER,
                    ErrorCodes::LaserErrors::LASER_INIT_FAILED,
                    "Failed to connect to laser device"
                );
                return false;
            }

            if (!VerifySafetyInterlock()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_LASER,
                    ErrorCodes::LaserErrors::LASER_SAFETY_INTERLOCK_TRIGGERED,
                    "Laser safety interlock is triggered",
                    "Check safety covers and interlocks before operation"
                );
                return false;
            }

            isEnabled = true;
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_LASER,
                ErrorCodes::LaserErrors::LASER_INIT_FAILED,
                std::string("Laser initialization exception: ") + e.what()
            );
            return false;
        }
    }

    bool SetPower(float powerMW) {
        if (!isEnabled) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_LASER,
                ErrorCodes::LaserErrors::LASER_DISCONNECTED,
                "Cannot set power: laser not enabled"
            );
            return false;
        }

        if (powerMW < 0.0f || powerMW > 500.0f) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_LASER,
                ErrorCodes::LaserErrors::LASER_INVALID_PARAMETER,
                "Invalid laser power value",
                "Requested: " + std::to_string(powerMW) + "mW, Valid range: 0-500mW"
            );
            return false;
        }

        try {
            if (!SendPowerCommand(powerMW)) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_LASER,
                    ErrorCodes::LaserErrors::LASER_POWER_CONTROL_FAILED,
                    "Failed to set laser power",
                    "Requested power: " + std::to_string(powerMW) + "mW"
                );
                return false;
            }
            currentPower = powerMW;
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_LASER,
                ErrorCodes::LaserErrors::LASER_POWER_CONTROL_FAILED,
                std::string("Laser power control exception: ") + e.what()
            );
            return false;
        }
    }

    void MonitorTemperature() {
        float tempC = ReadTemperature();
        if (tempC > 40.0f) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_LASER,
                ErrorCodes::LaserErrors::LASER_TEMPERATURE_WARNING,
                "Laser temperature exceeds safe threshold",
                "Current: " + std::to_string(tempC) + "°C, Max safe: 40°C"
            );
            // Could disable laser or reduce power here
        }
        currentTemp = tempC;
    }

private:
    bool ConnectLaser() { return true; }
    bool VerifySafetyInterlock() { return true; }
    bool SendPowerCommand(float power) { return true; }
    float ReadTemperature() { return currentTemp; }
};

// ============================================================================
// EXAMPLE 3: File I/O Integration
// ============================================================================

class ExampleFileHandler {
public:
    static bool ReadConfigFile(const std::string& filename, std::string& content) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_FILE_IO,
                ErrorCodes::FileIOErrors::FILE_NOT_FOUND,
                "Configuration file not found",
                "Path: " + filename
            );
            return false;
        }

        try {
            std::stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
            file.close();
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_FILE_IO,
                ErrorCodes::FileIOErrors::FILE_READ_FAILED,
                std::string("Failed to read configuration: ") + e.what(),
                "File: " + filename
            );
            return false;
        }
    }

    static bool WriteLogData(const std::string& filename, const std::string& data) {
        try {
            std::ofstream file(filename, std::ios::app);
            
            if (!file.is_open()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_FILE_IO,
                    ErrorCodes::FileIOErrors::FILE_OPEN_FAILED,
                    "Failed to open file for writing",
                    "Path: " + filename
                );
                return false;
            }

            file << data << "\n";
            if (!file.good()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_FILE_IO,
                    ErrorCodes::FileIOErrors::FILE_WRITE_FAILED,
                    "Failed to write data to file",
                    "Path: " + filename
                );
                file.close();
                return false;
            }

            file.close();
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_FILE_IO,
                ErrorCodes::FileIOErrors::FILE_WRITE_FAILED,
                std::string("File write exception: ") + e.what(),
                "File: " + filename
            );
            return false;
        }
    }
};

// ============================================================================
// EXAMPLE 4: Measurement System Integration
// ============================================================================

class ExampleMeasurementSystem {
private:
    bool isCalibrated;
    float minRange, maxRange;

public:
    ExampleMeasurementSystem() 
        : isCalibrated(false), minRange(0.0f), maxRange(100.0f) {}

    bool Initialize() {
        try {
            if (!Calibrate()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_MEASUREMENT,
                    ErrorCodes::MeasurementErrors::MEASUREMENT_INIT_FAILED,
                    "Failed to initialize measurement system",
                    "Calibration unsuccessful"
                );
                return false;
            }
            isCalibrated = true;
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_MEASUREMENT,
                ErrorCodes::MeasurementErrors::MEASUREMENT_INIT_FAILED,
                std::string("Measurement initialization exception: ") + e.what()
            );
            return false;
        }
    }

    bool TakeMeasurement(float& result) {
        if (!isCalibrated) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_MEASUREMENT,
                ErrorCodes::MeasurementErrors::MEASUREMENT_INIT_FAILED,
                "Cannot measure: system not calibrated"
            );
            return false;
        }

        try {
            if (!AcquireData(result)) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_MEASUREMENT,
                    ErrorCodes::MeasurementErrors::MEASUREMENT_ACQUISITION_FAILED,
                    "Failed to acquire measurement data"
                );
                return false;
            }

            if (result < minRange || result > maxRange) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_MEASUREMENT,
                    ErrorCodes::MeasurementErrors::MEASUREMENT_OUT_OF_RANGE,
                    "Measurement value out of expected range",
                    "Value: " + std::to_string(result) + 
                    ", Valid range: " + std::to_string(minRange) + 
                    " - " + std::to_string(maxRange)
                );
                return false;
            }

            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_MEASUREMENT,
                ErrorCodes::MeasurementErrors::MEASUREMENT_ANALYSIS_FAILED,
                std::string("Measurement analysis exception: ") + e.what()
            );
            return false;
        }
    }

private:
    bool Calibrate() { return true; }
    bool AcquireData(float& result) { result = 50.0f; return true; }
};

// ============================================================================
// EXAMPLE 5: Configuration System Integration
// ============================================================================

class ExampleConfigSystem {
public:
    static bool LoadConfiguration(const std::string& filename, 
                                 std::map<std::string, std::string>& config) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_CONFIG,
                    ErrorCodes::ConfigErrors::CONFIG_LOAD_FAILED,
                    "Failed to load configuration file",
                    "File: " + filename
                );
                return false;
            }

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                size_t delimiter = line.find('=');
                if (delimiter == std::string::npos) {
                    TabAdmin::ReportError(
                        ErrorCodes::CATEGORY_CONFIG,
                        ErrorCodes::ConfigErrors::CONFIG_INVALID_FORMAT,
                        "Invalid configuration format",
                        "Line: " + line
                    );
                    file.close();
                    return false;
                }

                std::string key = line.substr(0, delimiter);
                std::string value = line.substr(delimiter + 1);
                config[key] = value;
            }

            file.close();
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_CONFIG,
                ErrorCodes::ConfigErrors::CONFIG_LOAD_FAILED,
                std::string("Configuration load exception: ") + e.what(),
                "File: " + filename
            );
            return false;
        }
    }

    static bool ValidateConfiguration(const std::map<std::string, std::string>& config) {
        // Check required parameters
        const std::vector<std::string> required = {"device_type", "serial_number", "version"};
        
        for (const auto& param : required) {
            if (config.find(param) == config.end()) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_CONFIG,
                    ErrorCodes::ConfigErrors::CONFIG_MISSING_PARAMETER,
                    "Required configuration parameter missing",
                    "Parameter: " + param
                );
                return false;
            }
        }
        return true;
    }
};
