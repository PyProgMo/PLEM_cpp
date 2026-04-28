#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include "IDebuggable.h"
#include <map>
#include <string>
#include <thread>
#include <atomic>
#include <memory>

class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();

    // Delete copy and move constructors
    DebugConsole(const DebugConsole&) = delete;
    DebugConsole& operator=(const DebugConsole&) = delete;

    /**
     * @brief Register a component to be accessible via the console.
     * @param name The unique name/alias used to route commands to this component.
     * @param component Pointer to the debuggable component.
     */
    void registerComponent(const std::string& name, IDebuggable* component);

    /**
     * @brief Unregister a component.
     * @param name The name of the component to remove.
     */
    void unregisterComponent(const std::string& name);

    /**
     * @brief Execute a single command asynchronously or synchronously and return output.
     * @param line Full command line (e.g. "thorlabs init").
     */
    std::string executeCommand(const std::string& line);

    /**
     * @brief Start the background console thread.
     */
    void start();

    /**
     * @brief Stop the background console thread.
     */
    void stop();

private:
    void run();

    std::map<std::string, IDebuggable*> m_components;
    std::thread m_thread;
    std::atomic<bool> m_running;
};

#endif // DEBUG_CONSOLE_H
