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

    bool handleDebugCommand(const std::string& cmd, const std::vector<std::string>& args, std::ostream& out) override;

private:
    std::mutex m_andorMutex;
    std::map<std::string, std::shared_ptr<AndorCCD>> m_cameras;
    std::map<std::string, int> m_cameraIndices;
};
