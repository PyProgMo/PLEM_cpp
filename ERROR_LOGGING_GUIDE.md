# Error Logging System - Implementation Guide

## Overview
The Error Logging System provides comprehensive error tracking with:
- **Error Code Hierarchy**: Organized by category (System, Frontend, Backend, Devices, etc.)
- **Timestamp Logging**: Precise timestamps down to milliseconds with date
- **File Storage**: Automatic daily log files in `log/errors/` directory
- **Real-time Display**: Live error monitoring in Admin Tab
- **Thread-safe**: Mutex-protected for concurrent access

---

## Directory Structure
```
log/
└── errors/
    └── errors_YYYYMMDD.log    (Daily log files)
```

---

## Error Code Hierarchy

### Category System: 0xCCEEEE
- **CC** = Error Category (Hex)
- **EEEE** = Specific Error Code (Hex)

### Available Categories

| Code | Category | Description |
|------|----------|-------------|
| 0x01 | SYSTEM | OS/System errors |
| 0x02 | FRONTEND | UI/Frontend errors |
| 0x03 | BACKEND | Backend processing errors |
| 0x04 | FB_CONNECTOR | Firebase connector errors |
| 0x05 | CAMERA | Camera device errors |
| 0x06 | LASER | Laser source errors |
| 0x07 | SPECTROGRAPH | Spectrograph errors |
| 0x08 | POWERMETER | Power meter errors |
| 0x09 | STAGE | Nano stage errors |
| 0x0A | FILE_IO | File I/O errors |
| 0x0B | CONFIG | Configuration errors |
| 0x0C | MEASUREMENT | Measurement errors |

---

## Usage Examples

### Example 1: Basic Error Logging
```cpp
#include "ErrorLogger.h"
#include "ErrorCodes.h"

// Using error codes directly
ErrorLogger::GetInstance().LogError(
    ErrorCodes::CameraErrors::CAMERA_INIT_FAILED,
    "Failed to initialize camera device"
);
```

### Example 2: Error with Details
```cpp
ErrorLogger::GetInstance().LogError(
    ErrorCodes::CATEGORY_CAMERA,
    ErrorCodes::CameraErrors::CAMERA_DISCONNECTED,
    "Camera disconnected unexpectedly",
    "Device ID: CAM001, Connection timeout after 5 seconds"
);
```

### Example 3: Using Convenience Macros
```cpp
#include "ErrorLogger.h"

// Simple logging
LOG_ERROR(ErrorCodes::LaserErrors::LASER_POWER_CONTROL_FAILED, 
          "Failed to set laser power to 50%");

// With details
LOG_ERROR_DETAILS(ErrorCodes::LaserErrors::LASER_TEMPERATURE_WARNING,
                  "Laser temperature warning",
                  "Current temp: 45°C, Max safe: 40°C");

// By category
LOG_ERROR_CAT(ErrorCodes::CATEGORY_STAGE, 
              ErrorCodes::StageErrors::STAGE_OUT_OF_BOUNDS,
              "Stage position out of bounds");
```

### Example 4: Device Integration
```cpp
class CameraDevice {
public:
    bool Initialize() {
        try {
            // Initialize camera...
            if (/* initialization failed */) {
                TabAdmin::ReportError(
                    ErrorCodes::CATEGORY_CAMERA,
                    ErrorCodes::CameraErrors::CAMERA_INIT_FAILED,
                    "Camera initialization failed",
                    "Check USB connection and drivers"
                );
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            TabAdmin::ReportError(
                ErrorCodes::CATEGORY_CAMERA,
                ErrorCodes::CameraErrors::CAMERA_INIT_FAILED,
                std::string("Camera exception: ") + e.what()
            );
            return false;
        }
    }
};
```

---

## Log File Format

**File Name**: `errors_YYYYMMDD.log`

**Log Entry Format**:
```
[DD.MM.YYYY HH:MM:SS.mmm] [CATEGORY] [0xCCEEEE] Error Message | Details: Additional information
```

**Example Log File**:
```
[27.04.2026 14:23:45.123] [CAMERA] [0x050502] Failed to initialize camera device | Details: USB connection timeout
[27.04.2026 14:24:10.456] [LASER] [0x060605] Laser temperature warning | Details: Current temp: 45°C, Max safe: 40°C
[27.04.2026 14:25:30.789] [STAGE] [0x090901] Stage not found | Details: Check power and connections
[27.04.2026 14:26:15.234] [FILE_IO] [0x0A0A02] File read failed | Details: Configuration file corrupted
```

---

## Admin Tab Features

### Real-time Error Monitoring
- **Current Error Display**: Shows latest error information
- **Last Error Display**: Shows the last captured error with timestamp
- **Error History List**: Displays last 20 errors with timestamps
- **Auto-update**: Updates every 500ms

### Controls
1. **Log errors to file** checkbox: Enable/disable file logging
2. **Clear History** button: Clear error history in memory and display
3. **Log Directory**: Displays the active log directory path

### Display Elements
- Current error status
- Last error details
- Precise timestamp (date + time with milliseconds)
- Error history with timestamps
- Current log directory path

---

## Thread Safety

The ErrorLogger is **completely thread-safe**:
- Singleton pattern with mutex protection
- All operations are lock-protected
- Safe to call from multiple threads simultaneously
- No race conditions in file writing or history updates

---

## Retrieving Error Information

### Get Last Error
```cpp
auto lastError = ErrorLogger::GetInstance().GetLastError();
std::cout << "Time: " << lastError.first << std::endl;
std::cout << "Message: " << lastError.second << std::endl;
```

### Get Error History
```cpp
auto history = ErrorLogger::GetInstance().GetErrorHistory(20); // Last 20 errors
for (const auto& entry : history) {
    std::cout << entry.first << " - " << entry.second << std::endl;
}
```

### Get All Log Files
```cpp
auto logFiles = ErrorLogger::GetInstance().GetAllLogFiles();
for (const auto& file : logFiles) {
    std::cout << file << std::endl;
}
```

### Get Log Directory
```cpp
std::string logDir = ErrorLogger::GetInstance().GetLogDirectory();
```

---

## Best Practices

### 1. Use Appropriate Error Categories
Choose the right category for your error to enable quick filtering and analysis.

### 2. Include Descriptive Messages
Provide clear, actionable error messages that help identify the issue.

### 3. Add Contextual Details
Include relevant information like device IDs, values, or system state:
```cpp
TabAdmin::ReportError(
    ErrorCodes::CATEGORY_POWERMETER,
    ErrorCodes::PowerMeterErrors::POWERMETER_MEASUREMENT_FAILED,
    "Power meter measurement failed",
    "Reading: 25.4W, Expected range: 0-100W, Channel: 1"
);
```

### 4. Use Consistent Error Codes
Reuse the defined error codes from ErrorCodes.h rather than creating ad-hoc codes.

### 5. Log Early and Often
Log errors as soon as they're detected, not later in the process.

### 6. Avoid Sensitive Data
Don't log passwords, API keys, or other sensitive information.

---

## Error Code Reference

### System Errors (0x01)
- `0x010101`: Memory allocation failed
- `0x010102`: Initialization failed
- `0x010103`: Thread creation failed
- `0x010104`: Mutex lock failed

### Camera Errors (0x05)
- `0x050501`: Camera not found
- `0x050502`: Camera initialization failed
- `0x050503`: Capture failed
- `0x050504`: Camera disconnected
- `0x050505`: Timeout occurred
- `0x050506`: Invalid parameter

### Laser Errors (0x06)
- `0x060601`: Laser not found
- `0x060602`: Initialization failed
- `0x060603`: Power control failed
- `0x060604`: Laser disconnected
- `0x060605`: Temperature warning
- `0x060606`: Safety interlock triggered

*(See ErrorCodes.h for complete reference)*

---

## Integration Checklist

- [x] ErrorCodes.h created with comprehensive hierarchy
- [x] ErrorLogger.h created with singleton pattern
- [x] TabAdmin.h updated with live error monitoring
- [x] Thread-safe file I/O implementation
- [x] Timestamp precision to milliseconds
- [x] Daily log file rotation
- [x] Error history tracking
- [x] Admin Tab UI integration
- [x] Convenience macros for easy logging
- [x] Example usage and best practices

---

## Maintenance

### Log File Cleanup
Log files accumulate in `log/errors/`. Consider implementing a retention policy to delete old logs periodically (e.g., files older than 30 days).

### Example Cleanup (External Script)
```bash
# Delete log files older than 30 days
find log/errors -name "*.log" -mtime +30 -delete
```

---

## Troubleshooting

### Logging Not Working
1. Check if checkbox is enabled in Admin Tab
2. Verify `log/errors/` directory exists or can be created
3. Check file permissions
4. Verify disk space available

### Missing Errors
1. Ensure error reporting code is in place
2. Check that error logging is enabled (checkbox)
3. Verify error codes are correct
4. Check log file location

### Performance Impact
- Error logging has minimal impact (~<1ms per log operation)
- Background thread updates every 500ms (adjustable)
- Thread-safe design prevents blocking
