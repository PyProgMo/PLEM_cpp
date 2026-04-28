#include "../../include/DebugConsole.h"
#include <iostream>
#include <sstream>
#include <algorithm>

DebugConsole::DebugConsole() : m_running(false) {
}

DebugConsole::~DebugConsole() {
    stop();
}

void DebugConsole::registerComponent(const std::string& name, IDebuggable* component) {
    if (component) {
        m_components[name] = component;
    }
}

void DebugConsole::unregisterComponent(const std::string& name) {
    m_components.erase(name);
}

void DebugConsole::start() {
    if (m_running) return;
    
    m_running = true;
    m_thread = std::thread(&DebugConsole::run, this);
}

void DebugConsole::stop() {
    if (!m_running) return;
    
    m_running = false;
    // Note: std::getline is blocking, so the thread might not exit immediately 
    // unless input is provided or stdin is closed, but we detach or wait as needed.
    // For a cleaner shutdown, we check if joinable. Depending on OS, closing stdin helps.
    if (m_thread.joinable()) {
        m_thread.detach(); // Detach to prevent hanging on shutdown waiting for std::cin
    }
}

std::string DebugConsole::executeCommand(const std::string& line) {
    if (line.empty()) return "";

    std::istringstream iss(line);
    std::string componentName;
    std::string command;
    std::vector<std::string> args;

    iss >> componentName;

    if (componentName == "exit" || componentName == "quit") {
        m_running = false;
        return "Exiting debug console.\n";
    }

    if (componentName == "help") {
        std::ostringstream out;
        out << "Registered components:\n";
        for (const auto& pair : m_components) {
            out << "  - " << pair.first << "\n";
        }
        return out.str();
    }

    if (iss >> command) {
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        auto it = m_components.find(componentName);
        if (it != m_components.end()) {
            std::ostringstream out;
            bool handled = it->second->handleDebugCommand(command, args, out);
            if (!handled) {
                out << "Error: Component '" << componentName << "' did not recognize command '" << command << "'.\n";
            }
            return out.str();
        } else {
            return "Error: Unknown component '" + componentName + "'.\n";
        }
    } else {
        return "Error: Missing command for component '" + componentName + "'.\n";
    }
}

void DebugConsole::run() {
    std::string line;
    std::cout << "Interactive Debug Console Started.\n";
    std::cout << "Type '<component> <command> [args...]' or 'help' to list components.\n";
    
    while (m_running) {
        std::cout << "Debug> ";
        if (!std::getline(std::cin, line)) {
            // EOF or error
            break;
        }
        
        std::string output = executeCommand(line);
        if (!output.empty()) {
            std::cout << output;
        }
    }
}
