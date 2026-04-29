#include "FBConnector.h"
#include "../include/DebugConsole.h"
#include "../include/ErrorLogger.h"
#include <FL/Fl.H>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

FBConnector& FBConnector::get() {
    static FBConnector instance;
    return instance;
}

FBConnector::FBConnector() : m_running(false), m_debugConsole(nullptr), m_mainWindow(nullptr) {
    // Create debug log directory if it doesn't exist
    try {
        std::filesystem::create_directories("log/debug");
    } catch (...) {
        // Ignore failure to create directory
    }
}

FBConnector::~FBConnector() {
    stop();
}

int FBConnector::globalInteractionHandler(int event) {
    if (event == FL_RELEASE) {
        if (ErrorLogger::GetInstance().IsDeveloperMode()) {
            Fl_Widget* target = Fl::pushed(); // What widget did they push?
            if (target) {
                const char* lbl = target->label();
                std::string lblStr = (lbl && lbl[0] != '\0') ? lbl : "[Unlabeled Widget]";
                
                // Get widget class type to provide more context if needed
                FBConnector::get().logDeveloperInteraction("User clicked widget: '" + lblStr + "'");
            }
        }
    }
    return 0; // Return 0 to allow FLTK to continue processing the event normally
}

void FBConnector::logDeveloperInteraction(const std::string& interaction) {
    if (!ErrorLogger::GetInstance().IsDeveloperMode()) return;
    
    try {
        std::ofstream outFile("log/debug/devlog.log", std::ios::app);
        if (outFile.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
            
            outFile << "[" << ss.str() << "] [FBCONNECTOR] " << interaction << std::endl;
            outFile.close();
        }
    } catch (...) {
        // Silently fail if we can't write to devlog
    }
}

void FBConnector::start() {
    if (m_running) return;
    
    m_running = true;
    m_workerThread = std::thread(&FBConnector::workerLoop, this);
    
    // Wire up global interaction handler
    Fl::add_handler(globalInteractionHandler);
    
    // Start the UI update tick at 60 FPS (~0.0166 seconds)
    Fl::add_timeout(1.0 / 60.0, timerCallback, this);
}

void FBConnector::stop() {
    if (!m_running) return;
    
    m_running = false;
    m_backendCV.notify_all();
    
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
    
    Fl::remove_handler(globalInteractionHandler);
    Fl::remove_timeout(timerCallback, this);
}

void FBConnector::enqueueTask(std::function<void()> backendTask, Fl_Widget* lockingWidget) {
    if (lockingWidget) {
        // Disable the widget immediately on the UI thread
        lockingWidget->deactivate();
    }

    {
        std::lock_guard<std::mutex> lock(m_backendMutex);
        m_backendQueue.push([this, backendTask, lockingWidget]() {
            // Execute the heavy backend function
            if (backendTask) {
                backendTask();
            }
            
            // If we locked a widget, queue an update to re-enable it on the UI thread
            if (lockingWidget) {
                enqueueUIUpdate([lockingWidget]() {
                    lockingWidget->activate();
                });
            }
        });
    }
    m_backendCV.notify_one();
}

void FBConnector::enqueueUIUpdate(std::function<void()> uiTask) {
    std::lock_guard<std::mutex> lock(m_uiMutex);
    m_uiQueue.push(uiTask);
}

void FBConnector::workerLoop() {
    while (m_running) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(m_backendMutex);
            // Wait until there's a task or the engine stops
            m_backendCV.wait(lock, [this]() {
                return !m_backendQueue.empty() || !m_running;
            });

            if (!m_running && m_backendQueue.empty()) break;

            task = m_backendQueue.front();
            m_backendQueue.pop();
        }
        
        // Execute task outside the lock so other tasks can be queued
        if (task) {
            task();
        }
    }
}

void FBConnector::timerCallback(void* userdata) {
    FBConnector* connector = static_cast<FBConnector*>(userdata);
    connector->processUIQueue();
    
    // Schedule next tick (60 FPS)
    if (connector->m_running) {
        Fl::repeat_timeout(1.0 / 60.0, timerCallback, userdata);
    }
}

void FBConnector::processUIQueue() {
    // Quickly swap the queue out of the lock to prevent blocking enqueueing
    std::queue<std::function<void()>> localQueue;
    {
        std::lock_guard<std::mutex> lock(m_uiMutex);
        std::swap(m_uiQueue, localQueue);
    }

    // Execute UI updates on the main thread
    while (!localQueue.empty()) {
        auto& task = localQueue.front();
        if (task) {
            task();
        }
        localQueue.pop();
    }
}

void FBConnector::requestShutdown() {
    // Step 1: Backend shutdown - stop the DebugConsole
    enqueueTask([this]() {
        try {
            if (m_debugConsole) {
                m_debugConsole->stop();
            }
        } catch (const std::exception& e) {
            // Log any errors during DebugConsole shutdown
            std::string errMsg = std::string("Error stopping DebugConsole during shutdown: ") + e.what();
            ErrorLogger::GetInstance().LogError(0x00001000, "Shutdown Error", errMsg);
        }
    });
    
    // Step 2: Frontend shutdown - close window and exit
    enqueueUIUpdate([this]() {
        try {
            // Stop the FBConnector itself
            this->stop();
            
            // Close the main window if set
            if (m_mainWindow) {
                m_mainWindow->hide();
            }
            
            // Exit cleanly with code 0 (normal shutdown)
            exit(0);
        } catch (const std::exception& e) {
            // Try to log the error before exiting
            std::string errMsg = std::string("Error during frontend shutdown: ") + e.what();
            ErrorLogger::GetInstance().LogError(0x00001001, "Shutdown Error", errMsg);
            exit(0); // Exit anyway
        }
    });
}