#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_draw.H>

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
public:
    TabInit(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int cx = X + W / 2;
        
        // -------------------------------------------------------------
        // TOP LEFT: Device Status LEDs
        // -------------------------------------------------------------
        int b1_w = 130;
        int b1_h = 230;
        int b1_x = cx - b1_w - 5;
        int b1_y = Y + 20;

        Fl_Group* p1 = new Fl_Group(b1_x, b1_y, b1_w, b1_h);
        p1->box(FL_ENGRAVED_FRAME);
        
        const char* devNames[] = {"spectrograph", "lightsource", "powermeter", "nanostage", "cameras", "velleman", "ANDOR", "xeva"};
        for(int i=0; i<8; i++) {
            int yy = b1_y + 10 + i*26;
            FancyLED* led = new FancyLED(b1_x + 10, yy, 16, 16);
            if(i == 3) led->set_state(false); // Nanostage is green in screenshot
            
            Fl_Box* lbl = new Fl_Box(b1_x + 35, yy, b1_w - 40, 16, devNames[i]);
            lbl->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            lbl->labelsize(12);
        }
        p1->end();

        // -------------------------------------------------------------
        // TOP RIGHT: Device Init Toggles
        // -------------------------------------------------------------
        int b2_w = 130;
        int b2_h = 200;
        int b2_x = cx + 5;
        int b2_y = Y + 20;
        
        Fl_Group* p2 = new Fl_Group(b2_x, b2_y, b2_w, b2_h);
        p2->box(FL_ENGRAVED_FRAME);
        
        const char* initNames[] = {"spectrograph", "lightsource", "powermeter", "nanostage", "cameras", "xeva"};
        for(int i=0; i<6; i++) {
            int yy = b2_y + 15 + i*30;
            FancyToggle* tgl = new FancyToggle(b2_x + 10, yy, 110, 22, initNames[i]);
            tgl->labelsize(12);
        }
        p2->end();

        // -------------------------------------------------------------
        // BOTTOM LEFT: Camera Status Matrix
        // -------------------------------------------------------------
        int b3_w = 175;
        int b3_h = 130;
        int b3_x = cx - b3_w + 10;
        int b3_y = b1_y + b1_h + 15;

        Fl_Group* p3 = new Fl_Group(b3_x, b3_y, b3_w, b3_h);
        p3->box(FL_ENGRAVED_FRAME);
        
        // Matrix Headers
        Fl_Box* l1 = new Fl_Box(b3_x + 50, b3_y + 5, 30, 14, "Init");
        Fl_Box* l2 = new Fl_Box(b3_x + 85, b3_y + 5, 45, 14, "Cooling");
        Fl_Box* l3 = new Fl_Box(b3_x + 130, b3_y + 5, 60, 14, "Parameters");
        l1->labelsize(11); l2->labelsize(11); l3->labelsize(11);

        const char* camNames[] = {"Newton", "iDus", "Clara", "Xeva"};
        for(int i=0; i<4; i++) {
            int yy = b3_y + 25 + i*24;
            Fl_Box* cName = new Fl_Box(b3_x + 2, yy, 45, 16, camNames[i]);
            cName->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            cName->labelsize(12);
            
            // 3 LEDs per row
            new FancyLED(b3_x + 58, yy, 14, 14);
            new FancyLED(b3_x + 100, yy, 14, 14);
            new FancyLED(b3_x + 150, yy, 14, 14);
        }
        p3->end();

        // -------------------------------------------------------------
        // BOTTOM RIGHT: Camera Temperature Setup
        // -------------------------------------------------------------
        int b4_w = 120;
        int b4_h = 130;
        int b4_x = b3_x + b3_w + 5;
        int b4_y = b3_y;

        Fl_Group* p4 = new Fl_Group(b4_x, b4_y, b4_w, b4_h);
        p4->box(FL_ENGRAVED_FRAME);
        
        double defaultTemps[] = {-90, -70, -55, -70};
        const char* tempNames[] = {"iDus", "Newton", "Clara", "Xeva"};
        for(int i=0; i<4; i++) {
            int yy = b4_y + 10 + i*28;
            Fl_Box* cName = new Fl_Box(b4_x + 5, yy, 45, 20, tempNames[i]);
            cName->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            cName->labelsize(12);
            
            // Yellow toggle
            new FancyToggle(b4_x + 50, yy, 22, 20, "");
            
            // Value input 
            Fl_Value_Input* val = new Fl_Value_Input(b4_x + 75, yy, 40, 20);
            val->value(defaultTemps[i]);
            val->step(1);
            val->textsize(12);
            val->box(FL_DOWN_BOX);
        }
        p4->end();

        end();
    }
};
