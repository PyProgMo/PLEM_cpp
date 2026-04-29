#ifndef FBCONNECTOR_H
#define FBCONNECTOR_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

/**
 * @brief The Frontend-Backend Connector. 
 * Manages background asynchronous tasks to keep the UI thread safe and responsive.
 * Operates a worker thread for hardware/backend tasks and processes UI updates at 60 FPS.
 */
class FBConnector {
public:
    static FBConnector& get(); // Singleton for easy access across the frontend

    void start();
    void stop();

    /**
     * @brief Enqueue a backend task to be processed by the worker thread.
     * @param backendTask The function/lambda to execute asynchronously.
     * @param lockingWidget An optional FLTK widget (e.g., a button) to disable until the task finishes.
     */
    void enqueueTask(std::function<void()> backendTask, Fl_Widget* lockingWidget = nullptr);

    /**
     * @brief Enqueue a task to be processed on the main UI thread during the next 60fps tick.
     * Useful for updating GUI components safely from background threads.
     * @param uiTask The function to execute on the main thread.
     */
    void enqueueUIUpdate(std::function<void()> uiTask);

private:
    FBConnector();
    ~FBConnector();

    // Prevent copying
    FBConnector(const FBConnector&) = delete;
    FBConnector& operator=(const FBConnector&) = delete;

    void workerLoop();
    static void timerCallback(void* userdata);
    void processUIQueue();

    // Backend worker resources
    std::queue<std::function<void()>> m_backendQueue;
    std::mutex m_backendMutex;
    std::condition_variable m_backendCV;
    std::thread m_workerThread;
    std::atomic<bool> m_running;

    // Frontend (UI) resources
    std::queue<std::function<void()>> m_uiQueue;
    std::mutex m_uiMutex;
};

#endif // FBCONNECTOR_H