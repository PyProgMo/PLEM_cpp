#pragma once

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "../../standalone_devices/andorwrapper/cpp/AndorCCD.h"
#include "../../include/IDebuggable.h"

class AndorBackend : public IDebuggable {
public:
    AndorBackend();
    ~AndorBackend() override;

    bool initCamera(const std::string& name);
    bool deinitCamera(const std::string& name);
    bool isInitialized(const std::string& name);
    bool setCooling(const std::string& name, int temperature);

    // Spectrograph operations
    bool setSlitWidth(float width);
    bool setGrating(int index);
    bool setCentralWavelength(float wavelength);
    bool setFilter(int index);
    bool setShutter(int state); // 0 = open, 1 = closed, etc.
    bool resetGrating();
    bool resetSlit();

    bool handleDebugCommand(const std::string& cmd, const std::vector<std::string>& args, std::ostream& out) override;

private:
    std::mutex m_andorMutex;
    std::map<std::string, std::shared_ptr<AndorCCD>> m_cameras;
    std::map<std::string, int> m_cameraIndices;
    
    // Mock states for Spectrograph
    float m_slitWidth = 50.0f;
    int m_gratingIndex = 0;
    float m_centralWavelength = 1000.0f;
    int m_filterIndex = 0;
    int m_shutterState = 1;
};
