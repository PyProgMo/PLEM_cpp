#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include "LedButton.h"

class TabMeasurement : public Fl_Group {
    LedButton* btn_measure;
    LedButton* btn_abort;
    LedButton* btn_continuous_save;
    LedButton* btn_align_mode;
    LedButton* btn_nkt;
public:
    TabMeasurement(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int x_offset = X + 10;
        int y_offset = Y + 10;
        int btn_w = 130;
        int btn_h = 25;
        
        btn_measure = new LedButton(x_offset, y_offset, btn_w, btn_h, "measure");
        btn_abort = new LedButton(x_offset, y_offset + 30, btn_w, btn_h, "Abort");
        x_offset += 150;
        
        btn_continuous_save = new LedButton(x_offset, y_offset, btn_w + 20, btn_h, "continuous-save");
        btn_align_mode = new LedButton(x_offset, y_offset + 30, btn_w + 20, btn_h, "align-mode");
        x_offset += 170;
        
        btn_nkt = new LedButton(x_offset, y_offset + 30, btn_w, btn_h, "NKT");
        end();
    }
};
