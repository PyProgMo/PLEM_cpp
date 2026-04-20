#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>

class TabSpectrum : public Fl_Group {
public:
    TabSpectrum(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        // Main Spectrum Plot Area
        Fl_Box* plot_area = new Fl_Box(X + 10, Y + 10, W - 20, H - 20);
        plot_area->box(FL_DOWN_BOX);
        plot_area->color(FL_WHITE);
        plot_area->label("Spectrum Plot Area");
        plot_area->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        plot_area->labelfont(FL_HELVETICA_ITALIC);

        end();
    }
};
