#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "LedButton.h"

class TabDisplayOptions : public Fl_Group {
    LedButton* btn_autoscale;
    LedButton* btn_refresh;
    LedButton* btn_background;
public:
    TabDisplayOptions(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int x_offset = X + 10;
        int y_offset = Y + 10;
        int btn_w = 120;
        int btn_h = 25;
        
        btn_autoscale = new LedButton(x_offset, y_offset, btn_w, btn_h, "autoscale");
        x_offset += 140;
        
        btn_refresh = new LedButton(x_offset, y_offset, btn_w, btn_h, "refresh");
        x_offset += 140;
        
        btn_background = new LedButton(x_offset, y_offset, btn_w, btn_h, "background");
        
        end();
    }
};
