#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Tile.H>

// Left Tabs
#include "tabs/TabConfiguration.h"
#include "tabs/TabTimedMeasurement.h"
#include "tabs/TabAutoStage.h"
#include "tabs/TabPLEM.h"
#include "tabs/TabInit.h"
#include "tabs/TabAdmin.h"
#include "tabs/TabDebugger.h"

// Right Top Tabs
#include "tabs/TabSpectrum.h"

#include "tabs/TabImage.h"

// Right Bottom Tabs
#include "tabs/TabMeasurement.h"
#include "tabs/TabDisplayOptions.h"
#include "tabs/TabSave.h"
#include "tabs/TabBackground.h"
#include "tabs/TabAcquisitionSetup.h"
#include "tabs/TabLaserCmd.h"

class DragAwareTabs : public Fl_Tabs {
public:
    DragAwareTabs(int X, int Y, int W, int H, const char* L = 0) : Fl_Tabs(X, Y, W, H, L) {}

    int handle(int event) override {
        int handled = Fl_Tabs::handle(event);
        if (event == FL_DRAG || event == FL_RELEASE) {
            refresh_tab_headers();
        }
        return handled;
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Tabs::resize(X, Y, W, H);
        refresh_tab_headers();
    }

private:
    void refresh_tab_headers() {
        for (int i = 0; i < children(); ++i) {
            if (Fl_Widget* tab = child(i)) {
                tab->redraw_label();
            }
        }
        redraw();
    }
};

class FrontendConnection : public Fl_Window {
public:
    FrontendConnection(int W, int H, const char* title, DebugConsole* dbgConsole = nullptr) : Fl_Window(W, H, title) {
        // Main horizontal split
        Fl_Tile* tile = new Fl_Tile(0, 0, W, H);
        
        // Left pane (40% of width)
        int lw = W * 40 / 100;
        DragAwareTabs* ltabs = new DragAwareTabs(0, 0, lw, H);
        new TabConfiguration(0, 30, lw, H - 30, "Configuration");
        new TabTimedMeasurement(0, 30, lw, H - 30, "Timed Meas.");
        new TabAutoStage(0, 30, lw, H - 30, "Auto-Stage");
        new TabPLEM(0, 30, lw, H - 30, "PLEM");
        new TabInit(0, 30, lw, H - 30, "Init");
        new TabAdmin(0, 30, lw, H - 30, "Admin");
        new TabDebugger(0, 30, lw, H - 30, "Debugger", dbgConsole);
        ltabs->end();

        
        // Right pane (2/3 of width), separated into Top and Bottom
        Fl_Tile* rtile = new Fl_Tile(lw, 0, W - lw, H);
        int rth = H * 4 / 5; // Give 80% height to Spectrum/Image
        
        // Right Top (Displays / Blank Frame)
        DragAwareTabs* rttabs = new DragAwareTabs(lw, 0, W - lw, rth);
        new TabSpectrum(lw, 30, W - lw, rth - 30, "Spectrum");
        new TabImage(lw, 30, W - lw, rth - 30, "Image");
        rttabs->end();
        
        // Right Bottom (Controls)
        int rbw = W - lw;          // Total right bottom width
        int tab_w = rbw * 6 / 7;   // Tabs get 80% of horizontal space
        int laser_w = rbw - tab_w; // Laser controls get 20%
        
        Fl_Group* bot_grp = new Fl_Group(lw, rth, rbw, H - rth);
        
        DragAwareTabs* rbtabs = new DragAwareTabs(lw, rth, tab_w, H - rth);
        new TabMeasurement(lw, rth + 30, tab_w, H - rth - 30, "Measurement");
        new TabDisplayOptions(lw, rth + 30, tab_w, H - rth - 30, "Display Options");
        new TabSave(lw, rth + 30, tab_w, H - rth - 30, "Save");
        new TabBackground(lw, rth + 30, tab_w, H - rth - 30, "Background");
        new TabAcquisitionSetup(lw, rth + 30, tab_w, H - rth - 30, "Acq. Status"); // Match LabVIEW screenshot
        rbtabs->end();
        
        // Constant Laser Control Panel
        new TabLaserCmd(lw + tab_w, rth, laser_w, H - rth);
        
        bot_grp->end();
        
        rtile->end();
        tile->end();
        
        resizable(tile);
    }
};
