#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>

class TabImage : public Fl_Group {
public:
    TabImage(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        // Main Image Display Area
        Fl_Box* image_area = new Fl_Box(X + 10, Y + 10, W - 20, H - 20);
        image_area->box(FL_DOWN_BOX);
        image_area->color(FL_BLACK); // typically image backgrounds are black initially
        image_area->label("No Image Data");
        image_area->labelcolor(FL_WHITE);
        image_area->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        image_area->labelfont(FL_HELVETICA_ITALIC);

        end();
    }
};
