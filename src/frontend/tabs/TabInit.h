#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_draw.H>

#include "../../fbconnector/FBConnector.h"
#include "../../../include/ErrorLogger.h"
#include "../../../standalone_devices/thorlabs_powermeter/tl100d_reader/src/ThorlabsPM.h"

// Custom LED Widget to look like LabVIEW indicators
class FancyLED : public Fl_Widget {
    bool state_;
public:
    FancyLED(int x, int y, int w, int h, const char* L=0) : Fl_Widget(x,y,w,h,L), state_(false) {}
    void set_state(bool on) { state_ = on; redraw(); }
    void draw() override {
        fl_color(FL_BLACK);
        fl_pie(x(), y(), w(), h(), 0, 360);
        fl_color(state_ ? FL_GREEN : FL_RED);
        fl_pie(x()+1, y()+1, w()-2, h()-2, 0, 360);
    }
};

// Custom Switch looking like LabVIEW toggle
class FancyToggle : public Fl_Light_Button {
public:
    FancyToggle(int x, int y, int w, int h, const char* l=0) : Fl_Light_Button(x,y,w,h,l) {
        selection_color(fl_rgb_color(255, 204, 0)); // yellow-orange
        box(FL_ROUND_UP_BOX); 
        down_box(FL_ROUND_DOWN_BOX);
    }
};

class TabInit : public Fl_Group {
private:
    ThorlabsPM* powermeter;
    FancyLED* pm_led;
    FancyToggle* pm_toggle;

    static void pmToggleCallback(Fl_Widget* w, void* data) {
        TabInit* tab = static_cast<TabInit*>(data);
        tab->handlePmToggle();
    }

    void handlePmToggle() {
        if (!powermeter) return;
        
        bool willInit = pm_toggle->value(); // The desired state
        if (willInit) {
            FBConnector::get().enqueueTask([this]() {
                bool res = powermeter->init();
                FBConnector::get().enqueueUIUpdate([this, res]() {
                    pm_led->set_state(res);
                    pm_toggle->value(res ? 1 : 0);
                    if (!res) {
                        ErrorLogger::GetInstance().LogError(
                            ErrorCodes::CATEGORY_POWERMETER, 0x0001,
                            "Powermeter Init Failed", "Failed to initialize TLPM hardware."
                        );
                    }
                });
            }, pm_toggle);
        } else {
            FBConnector::get().enqueueTask([this]() {
                bool res = powermeter->deinit();
                FBConnector::get().enqueueUIUpdate([this, res]() {
                    // Update state regardless of deinit success
                    pm_led->set_state(powermeter->getStatus());
                    pm_toggle->value(powermeter->getStatus() ? 1 : 0);
                    
                    if (!res) {
                        ErrorLogger::GetInstance().LogError(
                            ErrorCodes::CATEGORY_POWERMETER, 0x0002,
                            "Powermeter Deinit Failed", "Failed to deinitialize TLPM hardware."
                        );
                    }
                });
            }, pm_toggle);
        }
    }

public:
    TabInit(int X, int Y, int W, int H, const char* L = 0, ThorlabsPM* pm = nullptr) 
        : Fl_Group(X, Y, W, H, L), powermeter(pm), pm_led(nullptr), pm_toggle(nullptr) {
        
        int cx = X + W / 2;
        
        // -------------------------------------------------------------
        // TOP LEFT: Device Status LEDs
        // -------------------------------------------------------------
        int row_height = 28;
        int max_rows = 9; // 9 rows for 9 init toggles
        
        int b1_w = 140;
        int b1_h = max_rows * row_height + 20;
        int b1_x = cx - b1_w - 10;
        int b1_y = Y + 20;

        Fl_Group* p1 = new Fl_Group(b1_x, b1_y, b1_w, b1_h);
        p1->box(FL_ROUNDED_BOX);
        p1->color(fl_rgb_color(240, 240, 240));
        
        const char* devNames[] = {"spectrograph", "lightsource", "powermeter", "nanostage", "cameras", "init Newton", "init iDus", "init Clara", "init Xeva"};
        for(int i=0; i<9; i++) {
            int yy = b1_y + 10 + i*row_height;
            FancyLED* led = new FancyLED(b1_x + 15, yy + 4, 18, 18);
            if(i == 3) led->set_state(false); // Nanostage is green in screenshot
            
            if (i == 2) pm_led = led; // Save powermeter LED

            Fl_Box* lbl = new Fl_Box(b1_x + 45, yy, b1_w - 50, 26, devNames[i]);
            lbl->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            lbl->labelsize(13);
            lbl->labelfont(FL_HELVETICA_BOLD);
        }
        p1->end();

        // -------------------------------------------------------------
        // TOP RIGHT: Device Init Toggles
        // -------------------------------------------------------------
        int b2_w = 140;
        int b2_h = b1_h;
        int b2_x = cx + 10;
        int b2_y = b1_y;
        
        Fl_Group* p2 = new Fl_Group(b2_x, b2_y, b2_w, b2_h);
        p2->box(FL_ROUNDED_BOX);
        p2->color(fl_rgb_color(240, 240, 240));
        
        const char* initNames[] = {"spectrograph", "lightsource", "powermeter", "nanostage", "cameras (all)", "init Newton", "init iDus", "init Clara", "init Xeva"};
        for(int i=0; i<9; i++) {
            int yy = b2_y + 10 + i*row_height;
            FancyToggle* tgl = new FancyToggle(b2_x + 10, yy + 2, 120, 24, initNames[i]);
            tgl->labelsize(12);
            tgl->labelfont(FL_HELVETICA_BOLD);

            if (i == 2) {
                pm_toggle = tgl;
                
                // Initialize default state if passed
                if (powermeter) {
                    bool status = powermeter->getStatus();
                    pm_toggle->value(status ? 1 : 0);
                    if (pm_led) pm_led->set_state(status);
                }
                
                pm_toggle->callback(pmToggleCallback, this);
            }
        }
        p2->end();

        // -------------------------------------------------------------
        // BOTTOM LEFT: Camera Temperature Setup (Moved above matrix)
        // -------------------------------------------------------------
        int b4_w = 140;
        int b4_h = 145;
        int b4_x = b1_x; // Align with left column
        int b4_y = b1_y + b1_h + 20;

        Fl_Group* p4 = new Fl_Group(b4_x, b4_y, b4_w, b4_h);
        p4->box(FL_ROUNDED_BOX);
        p4->color(fl_rgb_color(240, 240, 240));
        
        double defaultTemps[] = {-90, -70, -55, -70};
        const char* tempNames[] = {"iDus", "Newton", "Clara", "Xeva"};
        for(int i=0; i<4; i++) {
            int yy = b4_y + 15 + i*30;
            Fl_Box* cName = new Fl_Box(b4_x + 10, yy, 50, 20, tempNames[i]);
            cName->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            cName->labelsize(13);
            cName->labelfont(FL_HELVETICA_BOLD);
            
            // Yellow toggle
            FancyToggle* tgl = new FancyToggle(b4_x + 60, yy, 22, 22, "");
            tgl->value(1);
            
            // Value input 
            Fl_Value_Input* val = new Fl_Value_Input(b4_x + 85, yy, 45, 22);
            val->value(defaultTemps[i]);
            val->step(1);
            val->textsize(12);
            val->box(FL_DOWN_BOX);
        }
        p4->end();

        // -------------------------------------------------------------
        // BOTTOM: Camera Status Matrix (Moved below Temperature Setup)
        // -------------------------------------------------------------
        int b3_w = 230;
        int b3_h = 145;
        int b3_x = b1_x; // Align with left column
        int b3_y = b4_y + b4_h + 15;

        Fl_Group* p3 = new Fl_Group(b3_x, b3_y, b3_w, b3_h);
        p3->box(FL_ROUNDED_BOX);
        p3->color(fl_rgb_color(240, 240, 240));
        
        // Matrix Headers
        Fl_Box* l1 = new Fl_Box(b3_x + 75, b3_y + 10, 30, 14, "Init");
        Fl_Box* l2 = new Fl_Box(b3_x + 115, b3_y + 10, 45, 14, "Cooling");
        Fl_Box* l3 = new Fl_Box(b3_x + 165, b3_y + 10, 60, 14, "Param");
        l1->labelsize(12); l2->labelsize(12); l3->labelsize(12);
        l1->labelfont(FL_HELVETICA_BOLD); l2->labelfont(FL_HELVETICA_BOLD); l3->labelfont(FL_HELVETICA_BOLD);

        const char* camNames[] = {"Newton", "iDus", "Clara", "Xeva"};
        for(int i=0; i<4; i++) {
            int yy = b3_y + 35 + i*26;
            Fl_Box* cName = new Fl_Box(b3_x + 15, yy, 55, 16, camNames[i]);
            cName->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            cName->labelsize(13);
            cName->labelfont(FL_HELVETICA_BOLD);
            
            // 3 LEDs per row
            new FancyLED(b3_x + 83, yy, 16, 16);
            new FancyLED(b3_x + 130, yy, 16, 16);
            new FancyLED(b3_x + 187, yy, 16, 16);
        }
        p3->end();

        end();
    }
};
