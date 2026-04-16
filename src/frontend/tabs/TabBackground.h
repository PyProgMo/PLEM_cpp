#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>

class TabBackground : public Fl_Group {
public:
    TabBackground(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        Fl_Box* lbl_method = new Fl_Box(X + 20, Y + 20, 80, 25, "Method:");
        lbl_method->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Choice* ch_method = new Fl_Choice(X + 100, Y + 20, 120, 25);
        ch_method->add("single");
        ch_method->add("multiple");
        ch_method->value(0);

        Fl_Button* btn_record = new Fl_Button(X + 20, Y + 60, 100, 30, "Record BG");
        Fl_Button* btn_save = new Fl_Button(X + 130, Y + 60, 100, 30, "Save BG");

        end();
    }
};
