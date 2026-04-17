#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class LedButton : public Fl_Group {
    Fl_Button* btn;
    Fl_Box* led;
public:
    LedButton(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H) {
        led = new Fl_Box(X, Y + (H - 16) / 2, 16, 16);
        led->box(FL_OVAL_BOX);
        led->color(fl_rgb_color(0, 100, 0)); // Dark green (off state)
        
        btn = new Fl_Button(X + 22, Y, W - 22, H, L);
        end();
    }
    
    void set_led(bool on) {
        if (on) {
            led->color(fl_rgb_color(0, 255, 0)); // Bright green (on state)
        } else {
            led->color(fl_rgb_color(0, 100, 0)); // Dark green (off state)
        }
        led->redraw();
    }
    
    Fl_Button* button() { return btn; }
};
