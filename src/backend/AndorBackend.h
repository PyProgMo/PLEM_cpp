#pragma once

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "../../standalone_devices/andorwrapper/cpp/AndorCCD.h"

class AndorBackend {
public:
    AndorBackend();
    ~AndorBackend();

    bool initCamera(const std::string& name);
    bool deinitCamera(const std::string& name);
    bool isInitialized(const std::string& name);
    bool setCooling(const std::string& name, int temperature);

private:
    std::mutex m_andorMutex;
    std::map<std::string, std::shared_ptr<AndorCCD>> m_cameras;
    std::map<std::string, int> m_cameraIndices;
};
