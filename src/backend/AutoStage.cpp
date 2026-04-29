#include "AutoStage.h"
#include "../../include/ErrorLogger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

AutoStage::AutoStage() {
    coordinates.clear();
}

AutoStage::~AutoStage() {
    coordinates.clear();
}

bool AutoStage::loadCoordinatesFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ErrorLogger::GetInstance().LogError(
            ErrorCodes::CATEGORY_STAGE, 0x0001,
            "AutoStage: Failed to load coordinates",
            "Could not open file: " + filepath
        );
        return false;
    }

    coordinates.clear();
    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;
        if (line.empty()) continue;

        std::vector<float> coord;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            try {
                coord.push_back(std::stof(token));
            } catch (const std::exception& e) {
                ErrorLogger::GetInstance().LogError(
                    ErrorCodes::CATEGORY_STAGE, 0x0002,
                    "AutoStage: Invalid coordinate format in file",
                    "Line " + std::to_string(line_num) + ": " + line
                );
                file.close();
                return false;
            }
        }

        if (coord.size() == 3) {
            coordinates.push_back(coord);
        } else {
            ErrorLogger::GetInstance().LogError(
                ErrorCodes::CATEGORY_STAGE, 0x0006,
                "AutoStage: Coordinate must have exactly 3 values",
                "Line " + std::to_string(line_num) + ": found " + std::to_string(coord.size()) + " values"
            );
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

bool AutoStage::saveCoordinatesToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        ErrorLogger::GetInstance().LogError(
            ErrorCodes::CATEGORY_STAGE, 0x0003,
            "AutoStage: Failed to save coordinates",
            "Could not open file: " + filepath
        );
        return false;
    }

    for (const auto& coord : coordinates) {
        file << std::fixed << std::setprecision(3);
        file << coord[0] << "," << coord[1] << "," << coord[2] << "\n";
    }

    file.close();
    return true;
}

bool AutoStage::generateCoordinatesCube(float start_x, float end_x, float step_x,
                                          float start_y, float end_y, float step_y,
                                          float start_z, float end_z, float step_z) {
    if (step_x <= 0.0f || step_y <= 0.0f || step_z <= 0.0f) {
        ErrorLogger::GetInstance().LogError(
            ErrorCodes::CATEGORY_STAGE, 0x0007,
            "AutoStage: Step size must be positive",
            "step_x=" + std::to_string(step_x) + ", step_y=" + std::to_string(step_y) +
            ", step_z=" + std::to_string(step_z)
        );
        return false;
    }

    if (start_x > end_x || start_y > end_y || start_z > end_z) {
        ErrorLogger::GetInstance().LogError(
            ErrorCodes::CATEGORY_STAGE, 0x0008,
            "AutoStage: Start must be less than or equal to end",
            "X: [" + std::to_string(start_x) + ", " + std::to_string(end_x) + "], " +
            "Y: [" + std::to_string(start_y) + ", " + std::to_string(end_y) + "], " +
            "Z: [" + std::to_string(start_z) + ", " + std::to_string(end_z) + "]"
        );
        return false;
    }

    coordinates.clear();

    // Generate 3D coordinate cube
    // Note: Using epsilon for floating point comparison
    const float epsilon = 1e-6f;
    int count = 0;

    for (float x = start_x; x <= end_x + epsilon; x += step_x) {
        for (float y = start_y; y <= end_y + epsilon; y += step_y) {
            for (float z = start_z; z <= end_z + epsilon; z += step_z) {
                // Clamp to end values to avoid floating point overflow
                float final_x = (x > end_x) ? end_x : x;
                float final_y = (y > end_y) ? end_y : y;
                float final_z = (z > end_z) ? end_z : z;

                coordinates.push_back({final_x, final_y, final_z});
                count++;
            }
        }
    }

    if (coordinates.empty()) {
        ErrorLogger::GetInstance().LogError(
            ErrorCodes::CATEGORY_STAGE, 0x0009,
            "AutoStage: No coordinates generated",
            "Check step sizes and range parameters"
        );
        return false;
    }

    return true;
}

const std::vector<std::vector<float>>& AutoStage::getCoordinates() const {
    return coordinates;
}

bool AutoStage::getCoordinate(size_t index, float& x, float& y, float& z) const {
    if (index >= coordinates.size()) {
        return false;
    }

    x = coordinates[index][0];
    y = coordinates[index][1];
    z = coordinates[index][2];
    return true;
}

size_t AutoStage::getCoordinateCount() const {
    return coordinates.size();
}

void AutoStage::clearCoordinates() {
    coordinates.clear();
}

bool AutoStage::isValidCoordinateIndex(size_t index) const {
    return index < coordinates.size();
}

int AutoStage::estimateMeasurementTime(int secondsPerCoordinate) const {
    return (int)coordinates.size() * secondsPerCoordinate;
}
