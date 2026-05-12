#include "AndorBackend.h"
#include "../../include/ErrorLogger.h"

AndorBackend::AndorBackend() {
    // Map standard names to expected indices (or dummy indices if unknown).
    m_cameraIndices["Newton"] = 0;
    m_cameraIndices["iDus"] = 1;
    m_cameraIndices["Clara"] = 2;
    m_cameraIndices["Xeva"] = 3;
}

AndorBackend::~AndorBackend() {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    for (auto& pair : m_cameras) {
        try {
            pair.second->shutdown();
        } catch (...) {
            // Ignore on shutdown
        }
    }
}

bool AndorBackend::initCamera(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    
    if (m_cameras.find(name) == m_cameras.end()) {
        m_cameras[name] = std::make_shared<AndorCCD>(std::make_shared<ConsoleLogger>());
    }

    try {
        if (m_cameraIndices.count(name)) {
            m_cameras[name]->selectCamera(m_cameraIndices[name]);
        }
        m_cameras[name]->initialize("");
        return true;
    } catch (const std::exception& e) {
        ErrorLogger::GetInstance().LogError(
            ErrorCodes::CATEGORY_CAMERA, 0x1000, "Camera Init Failed", e.what()
        );
        return false;
    }
}

bool AndorBackend::deinitCamera(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    
    if (m_cameras.find(name) != m_cameras.end()) {
        try {
            m_cameras[name]->shutdown();
            return true;
        } catch (...) {
            return false;
        }
    }
    return true; // Was not initialized
}

bool AndorBackend::isInitialized(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    // Rough check based on existence in map for now.
    // We could add m_cameras[name]->getStatus() if we want it to be more precise
    return m_cameras.find(name) != m_cameras.end();
}

bool AndorBackend::setCooling(const std::string& name, int temperature) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    
    if (m_cameras.find(name) != m_cameras.end()) {
        try {
            m_cameras[name]->setTemperature(temperature);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}
