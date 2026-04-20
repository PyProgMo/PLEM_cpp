#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Choice.H>

class TabImage : public Fl_Group {
public:
    TabImage(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        // Main Image Display Area
        int img_w = W - 200; // Reserve 200 pixels for controls on the right
        Fl_Box* image_area = new Fl_Box(X + 10, Y + 10, img_w, H - 20);
        image_area->box(FL_DOWN_BOX);
        image_area->color(FL_BLACK); // typically image backgrounds are black initially
        image_area->label("No Image Data");
        image_area->labelcolor(FL_WHITE);
        image_area->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        image_area->labelfont(FL_HELVETICA_ITALIC);
        
        // Right side control panel
        int ctrl_x = X + 10 + img_w + 10;
        int ctrl_w = 170;
        
        Fl_Group* grp_controls = new Fl_Group(ctrl_x, Y + 10, ctrl_w, H - 20);
        grp_controls->box(FL_ENGRAVED_FRAME);
        
        Fl_Box* lbl_controls = new Fl_Box(ctrl_x, Y + 5, ctrl_w, 20, "Image Controls");
        lbl_controls->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        lbl_controls->labelfont(FL_HELVETICA_BOLD);

        int cy = Y + 35;
        
        Fl_Button* btn_save = new Fl_Button(ctrl_x + 10, cy, ctrl_w - 20, 30, "Save Image...");
        
        cy += 40;
        Fl_Button* btn_clear = new Fl_Button(ctrl_x + 10, cy, ctrl_w - 20, 30, "Clear Display");

        cy += 40;
        Fl_Light_Button* btn_auto_contract = new Fl_Light_Button(ctrl_x + 10, cy, ctrl_w - 20, 30, "Auto Contrast");
        btn_auto_contract->selection_color(FL_GREEN);
        
        cy += 50;
        Fl_Box* lbl_contrast = new Fl_Box(ctrl_x + 10, cy, ctrl_w - 20, 20, "Contrast Levels");
        lbl_contrast->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        cy += 20;
        Fl_Box* lbl_c_min = new Fl_Box(ctrl_x + 10, cy, 40, 25, "Min:");
        Fl_Float_Input* inp_c_min = new Fl_Float_Input(ctrl_x + 55, cy, ctrl_w - 65, 25);
        inp_c_min->value("0");
        
        cy += 30;
        Fl_Box* lbl_c_max = new Fl_Box(ctrl_x + 10, cy, 40, 25, "Max:");
        Fl_Float_Input* inp_c_max = new Fl_Float_Input(ctrl_x + 55, cy, ctrl_w - 65, 25);
        inp_c_max->value("65535");

        cy += 40;
        Fl_Box* lbl_colormap = new Fl_Box(ctrl_x + 10, cy, ctrl_w - 20, 20, "Colormap");
        lbl_colormap->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        cy += 20;
        Fl_Choice* ch_colormap = new Fl_Choice(ctrl_x + 10, cy, ctrl_w - 20, 25);
        ch_colormap->add("Grayscale");
        ch_colormap->add("Jet");
        ch_colormap->add("Viridis");
        ch_colormap->add("Hot");
        ch_colormap->value(0);

        grp_controls->end();
        end();
    }
};
