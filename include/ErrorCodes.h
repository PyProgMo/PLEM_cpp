#pragma once

namespace ErrorCodes {
    // Error code structure: 0xCCEEEE
    // CC = Category (00-FF)
    // EEEE = Error code (0000-FFFF)

    // ===== CATEGORY DEFINITIONS =====
    enum ErrorCategory {
        CATEGORY_SYSTEM = 0x01,          // System/OS errors
        CATEGORY_FRONTEND = 0x02,        // Frontend/UI errors
        CATEGORY_BACKEND = 0x03,         // Backend errors
        CATEGORY_FBCONNECTOR = 0x04,     // Firebase/FB connector
        CATEGORY_CAMERA = 0x05,          // Camera devices
        CATEGORY_LASER = 0x06,           // Laser source
        CATEGORY_SPECTROGRAPH = 0x07,    // Spectrograph
        CATEGORY_POWERMETER = 0x08,      // Power meter
        CATEGORY_STAGE = 0x09,           // Nano stage
        CATEGORY_FILE_IO = 0x0A,         // File I/O
        CATEGORY_CONFIG = 0x0B,          // Configuration
        CATEGORY_MEASUREMENT = 0x0C,     // Measurement
    };

    // ===== SYSTEM ERRORS =====
    enum SystemErrors {
        SYS_MEMORY_ALLOCATION_FAILED = 0x0101,
        SYS_INITIALIZATION_FAILED = 0x0102,
        SYS_THREAD_CREATION_FAILED = 0x0103,
        SYS_MUTEX_LOCK_FAILED = 0x0104,
    };

    // ===== FRONTEND/UI ERRORS =====
    enum FrontendErrors {
        FE_WINDOW_CREATION_FAILED = 0x0201,
        FE_WIDGET_INIT_FAILED = 0x0202,
        FE_DISPLAY_UPDATE_FAILED = 0x0203,
        FE_EVENT_HANDLER_ERROR = 0x0204,
        FE_INVALID_INPUT = 0x0205,
    };

    // ===== BACKEND ERRORS =====
    enum BackendErrors {
        BACKEND_INIT_FAILED = 0x0301,
        BACKEND_PROCESS_ERROR = 0x0302,
        BACKEND_COMMUNICATION_ERROR = 0x0303,
        BACKEND_INVALID_STATE = 0x0304,
    };

    // ===== FB CONNECTOR ERRORS =====
    enum FBConnectorErrors {
        FB_CONNECTION_FAILED = 0x0401,
        FB_AUTHENTICATION_FAILED = 0x0402,
        FB_DATA_SYNC_FAILED = 0x0403,
        FB_INVALID_RESPONSE = 0x0404,
    };

    // ===== CAMERA ERRORS =====
    enum CameraErrors {
        CAMERA_NOT_FOUND = 0x0501,
        CAMERA_INIT_FAILED = 0x0502,
        CAMERA_CAPTURE_FAILED = 0x0503,
        CAMERA_DISCONNECTED = 0x0504,
        CAMERA_TIMEOUT = 0x0505,
        CAMERA_INVALID_PARAMETER = 0x0506,
    };

    // ===== LASER ERRORS =====
    enum LaserErrors {
        LASER_NOT_FOUND = 0x0601,
        LASER_INIT_FAILED = 0x0602,
        LASER_POWER_CONTROL_FAILED = 0x0603,
        LASER_DISCONNECTED = 0x0604,
        LASER_TEMPERATURE_WARNING = 0x0605,
        LASER_SAFETY_INTERLOCK_TRIGGERED = 0x0606,
    };

    // ===== SPECTROGRAPH ERRORS =====
    enum SpectrographErrors {
        SPECTROGRAPH_NOT_FOUND = 0x0701,
        SPECTROGRAPH_INIT_FAILED = 0x0702,
        SPECTROGRAPH_CALIBRATION_FAILED = 0x0703,
        SPECTROGRAPH_DISCONNECTED = 0x0704,
        SPECTROGRAPH_GRATING_ERROR = 0x0705,
    };

    // ===== POWER METER ERRORS =====
    enum PowerMeterErrors {
        POWERMETER_NOT_FOUND = 0x0801,
        POWERMETER_INIT_FAILED = 0x0802,
        POWERMETER_MEASUREMENT_FAILED = 0x0803,
        POWERMETER_DISCONNECTED = 0x0804,
        POWERMETER_CALIBRATION_OVERDUE = 0x0805,
    };

    // ===== NANO STAGE ERRORS =====
    enum StageErrors {
        STAGE_NOT_FOUND = 0x0901,
        STAGE_INIT_FAILED = 0x0902,
        STAGE_MOVEMENT_FAILED = 0x0903,
        STAGE_DISCONNECTED = 0x0904,
        STAGE_OUT_OF_BOUNDS = 0x0905,
        STAGE_COLLISION_DETECTED = 0x0906,
    };

    // ===== FILE I/O ERRORS =====
    enum FileIOErrors {
        FILE_OPEN_FAILED = 0x0A01,
        FILE_READ_FAILED = 0x0A02,
        FILE_WRITE_FAILED = 0x0A03,
        FILE_NOT_FOUND = 0x0A04,
        FILE_PERMISSION_DENIED = 0x0A05,
        FILE_DISK_FULL = 0x0A06,
    };

    // ===== CONFIGURATION ERRORS =====
    enum ConfigErrors {
        CONFIG_LOAD_FAILED = 0x0B01,
        CONFIG_SAVE_FAILED = 0x0B02,
        CONFIG_INVALID_FORMAT = 0x0B03,
        CONFIG_MISSING_PARAMETER = 0x0B04,
        CONFIG_VALIDATION_FAILED = 0x0B05,
    };

    // ===== MEASUREMENT ERRORS =====
    enum MeasurementErrors {
        MEASUREMENT_INIT_FAILED = 0x0C01,
        MEASUREMENT_ACQUISITION_FAILED = 0x0C02,
        MEASUREMENT_ANALYSIS_FAILED = 0x0C03,
        MEASUREMENT_OUT_OF_RANGE = 0x0C04,
        MEASUREMENT_TIMEOUT = 0x0C05,
    };

    // Helper function to get error category
    inline ErrorCategory GetCategory(uint32_t errorCode) {
        return (ErrorCategory)((errorCode >> 8) & 0xFF);
    }

    // Helper function to create formatted error code
    inline uint32_t CreateErrorCode(ErrorCategory category, uint16_t specificError) {
        return (category << 8) | specificError;
    }
}
