#include "FBConnector.h"

FBConnector& FBConnector::get() {
    static FBConnector instance;
    return instance;
}

FBConnector::FBConnector() : m_running(false) {}

FBConnector::~FBConnector() {
    stop();
}

void FBConnector::start() {
    if (m_running) return;
    
    m_running = true;
    m_workerThread = std::thread(&FBConnector::workerLoop, this);
    
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