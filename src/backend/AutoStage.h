#pragma once

#include <vector>
#include <string>

/**
 * @brief AutoStage Backend - Handles coordinate management and computation
 * Manages loading, saving, and generating XYZ coordinates in micrometers (µm)
 */
class AutoStage {
private:
    // Internal coordinate storage (in micrometers)
    std::vector<std::vector<float>> coordinates;

public:
    AutoStage();
    ~AutoStage();

    /**
     * @brief Load coordinates from a text file
     * Expected format: X,Y,Z on each line (comma-separated floats)
     * @param filepath Path to the coordinate file
     * @return true if successful
     */
    bool loadCoordinatesFromFile(const std::string& filepath);

    /**
     * @brief Save coordinates to a text file
     * Format: X,Y,Z on each line (with 3 decimal places)
     * @param filepath Path to save the coordinate file
     * @return true if successful
     */
    bool saveCoordinatesToFile(const std::string& filepath) const;

    /**
     * @brief Generate coordinates from cycle parameters (XYZ cube)
     * Creates a 3D grid of coordinates from start to end with given step size
     * @param start_x X-axis start position (micrometers)
     * @param end_x X-axis end position (micrometers)
     * @param step_x X-axis step size (micrometers)
     * @param start_y Y-axis start position (micrometers)
     * @param end_y Y-axis end position (micrometers)
     * @param step_y Y-axis step size (micrometers)
     * @param start_z Z-axis start position (micrometers)
     * @param end_z Z-axis end position (micrometers)
     * @param step_z Z-axis step size (micrometers)
     * @return true if successful
     */
    bool generateCoordinatesCube(float start_x, float end_x, float step_x,
                                  float start_y, float end_y, float step_y,
                                  float start_z, float end_z, float step_z);

    /**
     * @brief Get the list of all coordinates
     * @return Reference to internal coordinates vector
     */
    const std::vector<std::vector<float>>& getCoordinates() const;

    /**
     * @brief Get coordinate at specific index
     * @param index Coordinate index
     * @param x Output X coordinate (micrometers)
     * @param y Output Y coordinate (micrometers)
     * @param z Output Z coordinate (micrometers)
     * @return true if index is valid
     */
    bool getCoordinate(size_t index, float& x, float& y, float& z) const;

    /**
     * @brief Get total number of coordinates
     * @return Number of loaded/generated coordinates
     */
    size_t getCoordinateCount() const;

    /**
     * @brief Clear all coordinates
     */
    void clearCoordinates();

    /**
     * @brief Validate coordinate at index
     * @param index Index to check
     * @return true if index is within valid range
     */
    bool isValidCoordinateIndex(size_t index) const;

    /**
     * @brief Estimate measurement time in seconds
     * @param secondsPerCoordinate Time to measure each coordinate (default 10s)
     * @return Estimated total time in seconds
     */
    int estimateMeasurementTime(int secondsPerCoordinate = 10) const;
};

