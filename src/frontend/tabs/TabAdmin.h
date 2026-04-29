#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include "../../../include/ErrorLogger.h"
#include "../../../include/ErrorCodes.h"
#include "../../fbconnector/FBConnector.h"
#include <thread>
#include <chrono>

class TabAdmin : public Fl_Group {
private:
    Fl_Check_Button* chk_log_errors;
    Fl_Check_Button* chk_dev_mode;
    Fl_Text_Buffer* buf_current;
    Fl_Text_Display* out_current;
    Fl_Text_Buffer* buf_last;
    Fl_Text_Display* out_last;
    Fl_Text_Buffer* buf_time;
    Fl_Text_Display* out_time;
    Fl_Browser* error_history_list;
    bool updateThreadRunning;
    std::thread* updateThread;

    // Callback to toggle error logging
    static void LoggingToggleCallback(Fl_Widget* w, void* data) {
        TabAdmin* tab = static_cast<TabAdmin*>(data);
        Fl_Check_Button* chk = static_cast<Fl_Check_Button*>(w);
        ErrorLogger::GetInstance().SetLoggingEnabled(chk->value() != 0);
    }

    // Callback to toggle developer mode
    static void DevModeToggleCallback(Fl_Widget* w, void* data) {
        Fl_Check_Button* chk = static_cast<Fl_Check_Button*>(w);
        ErrorLogger::GetInstance().SetDeveloperMode(chk->value() != 0);
    }

    // Callback for clear history button
    static void ClearHistoryCallback(Fl_Widget* w, void* data) {
        TabAdmin* tab = static_cast<TabAdmin*>(data);
        ErrorLogger::GetInstance().ClearErrorHistory();
        tab->UpdateErrorDisplay();
    }

    // Callback for shutdown button
    static void ShutdownCallback(Fl_Widget* w, void* data) {
        TabAdmin* tab = static_cast<TabAdmin*>(data);
        
        // Log shutdown initiation
        ErrorLogger::GetInstance().LogError(0x00000000, "Application Shutdown", 
                                           "shutdown initiated by user.");
        
        // Disable the shutdown button to prevent multiple clicks
        Fl_Button* btn = static_cast<Fl_Button*>(w);
        btn->deactivate();
        btn->label("Shutting down...");
        Fl::redraw();
        
        // Request graceful shutdown through FBConnector
        FBConnector::get().requestShutdown();
    }

    // Update error display
    void UpdateErrorDisplay() {
        auto lastError = ErrorLogger::GetInstance().GetLastError();
        
        // Update last error
        if (!lastError.second.empty()) {
            buf_last->text(lastError.second.c_str());
            buf_time->text(lastError.first.c_str());
        } else {
            buf_last->text("None.");
            buf_time->text("");
        }

        // Update current error (same as last for now, can be extended)
        buf_current->text(lastError.second.empty() ? "System operational. No active errors." 
                                                     : lastError.second.c_str());

        // Update error history list
        UpdateErrorHistoryList();
    }

    // Update error history list
    void UpdateErrorHistoryList() {
        auto history = ErrorLogger::GetInstance().GetErrorHistory(20);
        error_history_list->clear();
        
        for (const auto& entry : history) {
            std::string line = "[" + entry.first + "] " + entry.second;
            error_history_list->add(line.c_str());
        }
        
        // Scroll to bottom to show latest
        if (error_history_list->size() > 0) {
            error_history_list->bottomline(error_history_list->size());
        }
    }

    // Background thread for updating error display
    void ErrorUpdateThread() {
        while (updateThreadRunning) {
            UpdateErrorDisplay();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    // Static thread function wrapper
    static void* ThreadWrapper(void* data) {
        TabAdmin* tab = static_cast<TabAdmin*>(data);
        tab->ErrorUpdateThread();
        return nullptr;
    }

public:
    TabAdmin(int X, int Y, int W, int H, const char* L = 0) 
        : Fl_Group(X, Y, W, H, L), updateThreadRunning(true), updateThread(nullptr) {
        
        int cx = X + 20;
        int cy = Y + 40; // Push down slightly for the group label

        Fl_Group* grp_error = new Fl_Group(cx, cy, 470, 460, "Error Display & Logging");
        grp_error->box(FL_ENGRAVED_FRAME);
        grp_error->align(FL_ALIGN_TOP_LEFT);
        grp_error->labelfont(FL_HELVETICA_BOLD);
        
        int gx = cx + 20;
        int gy = cy + 20;

        // ===== LOGGING CONTROL =====
        chk_log_errors = new Fl_Check_Button(gx, gy, 150, 25, "Log errors to file");
        chk_log_errors->value(1); // Default enabled
        chk_log_errors->callback(LoggingToggleCallback, this);
        ErrorLogger::GetInstance().SetLoggingEnabled(true);
        
        // Developer mode checkbox
        chk_dev_mode = new Fl_Check_Button(gx + 160, gy, 150, 25, "Developer Mode");
        chk_dev_mode->value(1); // Default enabled
        chk_dev_mode->callback(DevModeToggleCallback, this);
        ErrorLogger::GetInstance().SetDeveloperMode(true);
        
        // Clear history button
        Fl_Button* btn_clear = new Fl_Button(gx, gy + 35, 100, 25, "Clear History");
        btn_clear->callback(ClearHistoryCallback, this);
        
        // Shutdown button
        Fl_Button* btn_shutdown = new Fl_Button(gx + 110, gy + 35, 100, 25, "Shutdown");
        btn_shutdown->color(FL_RED);
        btn_shutdown->labelcolor(FL_WHITE);
        btn_shutdown->callback(ShutdownCallback, this);
        
        gy += 70;
        
        // --- Current Error Display (Continuous) ---
        Fl_Box* lbl_current = new Fl_Box(gx, gy, 250, 20, "Current Error (Continuously Updating):");
        lbl_current->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_current->labelfont(FL_HELVETICA_BOLD);
        
        gy += 25;
        buf_current = new Fl_Text_Buffer();
        buf_current->text("System operational. No active errors.");
        out_current = new Fl_Text_Display(gx, gy, 300, 80);
        out_current->buffer(buf_current);
        out_current->color(FL_LIGHT2);

        gy += 95;
        
        // --- Last Error Display ("Snapped") ---
        Fl_Box* lbl_last = new Fl_Box(gx, gy, 250, 20, "Last Error:");
        lbl_last->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_last->labelfont(FL_HELVETICA_BOLD);
        
        // --- Timestamp ---
        Fl_Box* lbl_time = new Fl_Box(gx + 310, gy, 130, 20, "Timestamp:");
        lbl_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_time->labelfont(FL_HELVETICA_BOLD);

        gy += 25;
        buf_last = new Fl_Text_Buffer();
        buf_last->text("None.");
        out_last = new Fl_Text_Display(gx, gy, 300, 80);
        out_last->buffer(buf_last);
        out_last->color(FL_LIGHT2);

        buf_time = new Fl_Text_Buffer();
        buf_time->text("00:00:00.000\nDD.MM.YYYY");
        out_time = new Fl_Text_Display(gx + 310, gy, 120, 80);
        out_time->buffer(buf_time);
        out_time->color(FL_LIGHT2);

        gy += 95;

        // --- Error History List ---
        Fl_Box* lbl_history = new Fl_Box(gx, gy, 300, 20, "Error History (Last 20):");
        lbl_history->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_history->labelfont(FL_HELVETICA_BOLD);

        gy += 25;
        error_history_list = new Fl_Browser(gx, gy, 430, 360);
        error_history_list->type(FL_MULTI_BROWSER);
        error_history_list->color(FL_LIGHT2);

        gy += 130;

        // --- Log Directory Info ---
        Fl_Box* lbl_info = new Fl_Box(gx, gy, 200, 20);
        std::string logPath = "Log Path: " + ErrorLogger::GetInstance().GetLogDirectory();
        lbl_info->label(logPath.c_str());
        lbl_info->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_info->labelfont(FL_HELVETICA);
        lbl_info->labelsize(10);

        grp_error->end();

        end();

        // Start background update thread
        updateThread = new std::thread(&TabAdmin::ErrorUpdateThread, this);
    }

    ~TabAdmin() {
        updateThreadRunning = false;
        if (updateThread && updateThread->joinable()) {
            updateThread->join();
        }
        delete updateThread;
    }

    // Public method to log errors from other components
    static void ReportError(uint32_t errorCode, const std::string& message, 
                           const std::string& details = "") {
        ErrorLogger::GetInstance().LogError(errorCode, message, details);
    }

    // Convenience method for error reporting
    static void ReportError(ErrorCodes::ErrorCategory category, uint16_t specificError,
                           const std::string& message, const std::string& details = "") {
        ErrorLogger::GetInstance().LogError(category, specificError, message, details);
    }
};
