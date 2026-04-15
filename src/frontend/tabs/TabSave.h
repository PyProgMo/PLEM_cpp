#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
class TabSave : public Fl_Group {
public:
    TabSave(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        Fl_Box* box = new Fl_Box(X + 10, Y + 10, W - 20, H - 20, "${t}");
        box->box(FL_ENGRAVED_BOX);
        box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        end();
    }
};
