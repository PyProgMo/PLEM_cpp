#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>

class LedIndicator : public Fl_Group {
    Fl_Box* led;
    Fl_Box* label;
public:
    LedIndicator(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H) {
        led = new Fl_Box(X, Y + (H - 16) / 2, 16, 16);
        led->box(FL_OVAL_BOX);
        led->color(fl_rgb_color(0, 100, 0)); // Default off
        
        if (L) {
            label = new Fl_Box(X + 22, Y, W - 22, H, L);
            label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        } else {
            label = nullptr;
        }
        end();
    }
    
    void set_led(bool on) {
        if (on) {
            led->color(fl_rgb_color(0, 255, 0)); 
        } else {
            led->color(fl_rgb_color(0, 100, 0)); 
        }
        led->redraw();
    }
};
