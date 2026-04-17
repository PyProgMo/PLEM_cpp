#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include "LedButton.h"

class TabSave : public Fl_Group {
    LedButton* btn_save_spectrum;
    Fl_Input* input_filename;
public:
    TabSave(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int x_offset = X + 90; // leave space for "File Name:" label on the left
        int y_offset = Y + 10;
        int input_w = 150;
        int btn_w = 180;
        int ui_h = 25;
        
        input_filename = new Fl_Input(x_offset, y_offset, input_w, ui_h, "File Name:");
        input_filename->value("spectrum");
        
        btn_save_spectrum = new LedButton(x_offset + input_w + 30, y_offset, btn_w, ui_h, "Save current Spectrum");
        
        end();
    }
};
