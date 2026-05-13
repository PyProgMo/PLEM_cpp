#include "AndorBackend.h"
#include "../../include/ErrorLogger.h"

AndorBackend::AndorBackend() {
    // Map standard names to expected indices (or dummy indices if unknown).
    m_cameraIndices["Newton"] = 0;
    m_cameraIndices["iDus"] = 1;
    m_cameraIndices["Clara"] = 2;
    m_cameraIndices["Xeva"] = 3;
    m_cameraIndices["spectrograph"] = 0; // Assuming spectrograph is linked to primary Newton index, or handled internally
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

bool AndorBackend::setSlitWidth(float width) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    if (width < 10.0f || width > 2500.0f) {
        ErrorLogger::GetInstance().LogError(ErrorCodes::CATEGORY_SPECTROGRAPH, 0x1101, "Slit Width Out of Range", "Width must be between 10µm and 2500µm");
        return false;
    }
    // Simulate setting hardware slit width
    m_slitWidth = width;
    return true;
}

bool AndorBackend::setGrating(int index) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    if (index < 0 || index > 2) {
        ErrorLogger::GetInstance().LogError(ErrorCodes::CATEGORY_SPECTROGRAPH, 0x1102, "Invalid Grating Index", "Selected grating does not exist");
        return false;
    }
    m_gratingIndex = index;
    return true;
}

bool AndorBackend::setCentralWavelength(float wavelength) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    if (wavelength < 200.0f || wavelength > 1200.0f) {
        ErrorLogger::GetInstance().LogError(ErrorCodes::CATEGORY_SPECTROGRAPH, 0x1103, "Wavelength Out of Bounds", "Central wavelength must be between 200nm and 1200nm");
        return false;
    }
    m_centralWavelength = wavelength;
    return true;
}

bool AndorBackend::setFilter(int index) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    if (index < 0 || index > 2) {
        ErrorLogger::GetInstance().LogError(ErrorCodes::CATEGORY_SPECTROGRAPH, 0x1104, "Invalid Filter Index", "Filter index out of bounds");
        return false;
    }
    m_filterIndex = index;
    return true;
}

bool AndorBackend::setShutter(int state) {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    m_shutterState = state;
    return true;
}

bool AndorBackend::resetGrating() {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    // Simulate reset behavior
    m_gratingIndex = 0;
    return true;
}

bool AndorBackend::resetSlit() {
    std::lock_guard<std::mutex> lock(m_andorMutex);
    // Simulate reset behavior
    m_slitWidth = 50.0f;
    return true;
}

bool AndorBackend::handleDebugCommand(const std::string& cmd, const std::vector<std::string>& args, std::ostream& out) {
    if (cmd == "init") {
        if (args.empty()) {
            out << "Usage: andor init <camera_name>" << std::endl;
            return true;
        }
        std::string name = args[0];
        if (initCamera(name)) {
            out << "Camera '" << name << "' initialized successfully." << std::endl;
        } else {
            out << "Failed to initialize camera '" << name << "'." << std::endl;
        }
        return true;
    } 
    else if (cmd == "deinit") {
        if (args.empty()) {
            out << "Usage: andor deinit <camera_name>" << std::endl;
            return true;
        }
        std::string name = args[0];
        if (deinitCamera(name)) {
            out << "Camera '" << name << "' deinitialized." << std::endl;
        } else {
            out << "Failed to deinitialize camera '" << name << "'." << std::endl;
        }
        return true;
    }
    else if (cmd == "temp") {
        if (args.size() < 2) {
            out << "Usage: andor temp <camera_name> <target_temp>" << std::endl;
            return true;
        }
        std::string name = args[0];
        int temp = std::stoi(args[1]);
        if (setCooling(name, temp)) {
            out << "Cooling for '" << name << "' set to " << temp << " degrees." << std::endl;
        } else {
            out << "Failed to set cooling for '" << name << "'." << std::endl;
        }
        return true;
    }
    else if (cmd == "status") {
        if (args.empty()) {
            out << "Usage: andor status <camera_name>" << std::endl;
            return true;
        }
        std::string name = args[0];
        std::lock_guard<std::mutex> lock(m_andorMutex);
        if (m_cameras.find(name) != m_cameras.end()) {
            try {
                auto tempInfo = m_cameras[name]->getTemperature();
                out << "Camera '" << name << "' status: Initialized" << std::endl;
                out << "Temperature: " << tempInfo.first << " (" << tempInfo.second << ")" << std::endl;
            } catch (const std::exception& e) {
                out << "Error reading status for '" << name << "': " << e.what() << std::endl;
            }
        } else {
            out << "Camera '" << name << "' is not initialized." << std::endl;
        }
        out << "Spectrograph Status: Slit=" << m_slitWidth << " Grating=" << m_gratingIndex 
            << " CWL=" << m_centralWavelength << " Filter=" << m_filterIndex << " Shutter=" << m_shutterState << std::endl;
        return true;
    }
    else if (cmd == "set_slit") {
        if (args.empty()) {
            out << "Usage: andor set_slit <width>" << std::endl;
            return true;
        }
        float w = std::stof(args[0]);
        if (setSlitWidth(w)) out << "Slit width set to " << w << std::endl;
        else out << "Failed to set slit width." << std::endl;
        return true;
    }
    else if (cmd == "set_wl") {
        if (args.empty()) {
            out << "Usage: andor set_wl <nm>" << std::endl;
            return true;
        }
        float wl = std::stof(args[0]);
        if (setCentralWavelength(wl)) out << "Central wavelength set to " << wl << " nm" << std::endl;
        else out << "Failed to set wavelength." << std::endl;
        return true;
    }
    
    return false;
}
