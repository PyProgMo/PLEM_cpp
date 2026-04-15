#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class TabLaserCmd : public Fl_Group {
public:
    TabLaserCmd(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        box(FL_ENGRAVED_BOX);
        
        int btnW = W * 0.8;
        int btnX = X + (W - btnW) / 2;
        int btnY = Y + H / 4;
        
        Fl_Button* laser_on = new Fl_Button(btnX, btnY, btnW, 30, "laser on");
        laser_on->color(fl_rgb_color(0, 100, 0)); // Dark green
        laser_on->labelcolor(FL_WHITE);
        
        Fl_Button* shutdown_btn = new Fl_Button(btnX, btnY + 40, btnW, 30, "shutdown");
        
        end();
    }
};
