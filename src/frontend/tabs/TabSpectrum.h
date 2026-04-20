#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Light_Button.H>

class TabSpectrum : public Fl_Group {
public:
    TabSpectrum(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        // Main Spectrum Plot Area
        int plot_w = W - 180; // Reserve 180 pixels for controls on the right
        Fl_Box* plot_area = new Fl_Box(X + 10, Y + 10, plot_w, H - 20);
        plot_area->box(FL_DOWN_BOX);
        plot_area->color(FL_WHITE);
        plot_area->label("Spectrum Plot Area");
        plot_area->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        plot_area->labelfont(FL_HELVETICA_ITALIC);
        
        // Right side control panel
        int ctrl_x = X + 10 + plot_w + 10;
        int ctrl_w = 150;
        
        Fl_Group* grp_controls = new Fl_Group(ctrl_x, Y + 10, ctrl_w, H - 20);
        grp_controls->box(FL_ENGRAVED_FRAME);
        
        Fl_Box* lbl_controls = new Fl_Box(ctrl_x, Y + 5, ctrl_w, 20, "Plot Controls");
        lbl_controls->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        lbl_controls->labelfont(FL_HELVETICA_BOLD);

        int cy = Y + 35;
        
        Fl_Light_Button* btn_autoscale = new Fl_Light_Button(ctrl_x + 10, cy, ctrl_w - 20, 30, "Auto Scale");
        btn_autoscale->selection_color(FL_GREEN);
        
        cy += 40;
        Fl_Button* btn_clear = new Fl_Button(ctrl_x + 10, cy, ctrl_w - 20, 30, "Clear Plot");
        
        cy += 40;
        Fl_Button* btn_save = new Fl_Button(ctrl_x + 10, cy, ctrl_w - 20, 30, "Save Data...");

        cy += 50;
        Fl_Box* lbl_x_axis = new Fl_Box(ctrl_x + 10, cy, ctrl_w - 20, 20, "X-Axis (Wavelength)");
        lbl_x_axis->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        cy += 20;
        Fl_Box* lbl_x_min = new Fl_Box(ctrl_x + 10, cy, 30, 25, "Min:");
        Fl_Float_Input* inp_x_min = new Fl_Float_Input(ctrl_x + 45, cy, ctrl_w - 55, 25);
        inp_x_min->value("400.0");
        
        cy += 30;
        Fl_Box* lbl_x_max = new Fl_Box(ctrl_x + 10, cy, 30, 25, "Max:");
        Fl_Float_Input* inp_x_max = new Fl_Float_Input(ctrl_x + 45, cy, ctrl_w - 55, 25);
        inp_x_max->value("1100.0");

        cy += 40;
        Fl_Box* lbl_y_axis = new Fl_Box(ctrl_x + 10, cy, ctrl_w - 20, 20, "Y-Axis (Intensity)");
        lbl_y_axis->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        cy += 20;
        Fl_Box* lbl_y_min = new Fl_Box(ctrl_x + 10, cy, 30, 25, "Min:");
        Fl_Float_Input* inp_y_min = new Fl_Float_Input(ctrl_x + 45, cy, ctrl_w - 55, 25);
        inp_y_min->value("0.0");
        
        cy += 30;
        Fl_Box* lbl_y_max = new Fl_Box(ctrl_x + 10, cy, 30, 25, "Max:");
        Fl_Float_Input* inp_y_max = new Fl_Float_Input(ctrl_x + 45, cy, ctrl_w - 55, 25);
        inp_y_max->value("65535.0");

        grp_controls->end();
        end();
    }
};
