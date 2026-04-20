#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Choice.H>
#include "LedButton.h"

class TabDisplayOptions : public Fl_Group {
    LedButton* btn_autoscale;
    LedButton* btn_refresh;
    LedButton* btn_background;
public:
    TabDisplayOptions(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int x_offset = X + 10;
        int y_offset = Y + 5;
        int btn_w = 120;
        int btn_h = 20;
        
        btn_autoscale = new LedButton(x_offset, y_offset, btn_w, btn_h, "autoscale");
        x_offset += 140;
        
        btn_refresh = new LedButton(x_offset, y_offset, btn_w, btn_h, "refresh");
        x_offset += 140;
        
        btn_background = new LedButton(x_offset, y_offset, btn_w, btn_h, "background");
        
        // Group for Image Controls
        y_offset += 30;
        Fl_Group* grp_img = new Fl_Group(X + 10, y_offset, 250, 100, "Image Controls");
        grp_img->box(FL_ENGRAVED_FRAME);
        grp_img->align(FL_ALIGN_TOP_LEFT);
        
        Fl_Light_Button* btn_auto_contract = new Fl_Light_Button(X + 20, y_offset + 5, 220, 20, "Auto Contrast");
        btn_auto_contract->selection_color(FL_GREEN);
        
        Fl_Box* lbl_c_min = new Fl_Box(X + 20, y_offset + 35, 35, 20, "Min:");
        Fl_Float_Input* inp_c_min = new Fl_Float_Input(X + 60, y_offset + 35, 70, 20);
        inp_c_min->value("0");
        
        Fl_Box* lbl_c_max = new Fl_Box(X + 135, y_offset + 35, 40, 20, "Max:");
        Fl_Float_Input* inp_c_max = new Fl_Float_Input(X + 180, y_offset + 35, 60, 20);
        inp_c_max->value("65535");

        Fl_Box* lbl_colormap = new Fl_Box(X + 20, y_offset + 65, 80, 20, "Colormap:");
        lbl_colormap->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_colormap = new Fl_Choice(X + 100, y_offset + 65, 140, 20);
        ch_colormap->add("Grayscale");
        ch_colormap->add("Jet");
        ch_colormap->add("Viridis");
        ch_colormap->add("Hot");
        ch_colormap->value(0);
        grp_img->end();

        // Group for Spectrum Controls
        Fl_Group* grp_spec = new Fl_Group(X + 280, y_offset, 250, 100, "Spectrum Controls");
        grp_spec->box(FL_ENGRAVED_FRAME);
        grp_spec->align(FL_ALIGN_TOP_LEFT);
        
        Fl_Box* lbl_x_axis = new Fl_Box(X + 290, y_offset + 5, 230, 20, "X-Axis (Wavelength)");
        lbl_x_axis->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Box* lbl_x_min = new Fl_Box(X + 290, y_offset + 25, 35, 20, "Min:");
        Fl_Float_Input* inp_x_min = new Fl_Float_Input(X + 330, y_offset + 25, 70, 20);
        inp_x_min->value("400.0");
        
        Fl_Box* lbl_x_max = new Fl_Box(X + 410, y_offset + 25, 40, 20, "Max:");
        Fl_Float_Input* inp_x_max = new Fl_Float_Input(X + 450, y_offset + 25, 60, 20);
        inp_x_max->value("1100.0");

        Fl_Box* lbl_y_axis = new Fl_Box(X + 290, y_offset + 50, 230, 20, "Y-Axis (Intensity)");
        lbl_y_axis->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Box* lbl_y_min = new Fl_Box(X + 290, y_offset + 70, 35, 20, "Min:");
        Fl_Float_Input* inp_y_min = new Fl_Float_Input(X + 330, y_offset + 70, 70, 20);
        inp_y_min->value("0.0");
        
        Fl_Box* lbl_y_max = new Fl_Box(X + 410, y_offset + 70, 40, 20, "Max:");
        Fl_Float_Input* inp_y_max = new Fl_Float_Input(X + 450, y_offset + 70, 60, 20);
        inp_y_max->value("65535.0");
        grp_spec->end();

        end();
    }
};
