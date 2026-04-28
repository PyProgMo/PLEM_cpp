#ifndef I_DEBUGGABLE_H
#define I_DEBUGGABLE_H

#include <string>
#include <vector>
#include <ostream>

/**
 * @brief Interface for components that can be interacted with via the Debug Console.
 */
class IDebuggable {
public:
    virtual ~IDebuggable() = default;

    /**
     * @brief Process a debug command sent to this component.
     * @param cmd The main command string.
     * @param args The arguments provided with the command.
     * @param out The output stream to write responses to.
     * @return true if the command was recognized and handled, false otherwise.
     */
    virtual bool handleDebugCommand(const std::string& cmd, const std::vector<std::string>& args, std::ostream& out) = 0;
};

#endif // I_DEBUGGABLE_H
