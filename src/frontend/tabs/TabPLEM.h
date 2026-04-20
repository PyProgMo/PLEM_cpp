#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#include "LedButton.h"
#include "LedIndicator.h"

class PLEMColorGraph : public Fl_Widget {
public:
    PLEMColorGraph(int X, int Y, int W, int H, const char* L=0) : Fl_Widget(X,Y,W,H,L) {}
    void draw() override {
        // Draw main background
        fl_color(fl_rgb_color(220, 220, 220)); // match panel gray
        fl_rectf(x(), y(), w(), h());

        int pad_l = 60; // Left padding for Y axis
        int pad_b = 30; // Bottom padding for X axis
        int pad_r = 70; // Right padding for Colorbar
        int plot_x = x() + pad_l;
        int plot_y = y() + 5;
        int plot_w = w() - pad_l - pad_r;
        int plot_h = h() - pad_b - 5;

        // Draw graph area
        fl_color(FL_BLACK);
        fl_rectf(plot_x, plot_y, plot_w, plot_h);

        fl_color(FL_BLACK);
        fl_line_style(FL_SOLID, 2);
        
        // Draw Ticks X
        fl_font(FL_HELVETICA, 11);
        const char* x_ticks[] = {"811.09", "850", "900", "950", "1000", "1050", "1087.44"};
        int num_x_ticks = 7;
        for (int i = 0; i < num_x_ticks; ++i) {
            int tx = plot_x + i * (plot_w / (num_x_ticks - 1.0f));
            fl_line(tx, plot_y + plot_h, tx, plot_y + plot_h + 5);
            fl_draw(x_ticks[i], tx - 15, plot_y + plot_h + 18);
        }
        
        // Draw Ticks Y
        const char* y_ticks[] = {"545", "560", "570", "580", "590", "600", "610", "620", "630", "645"};
        int num_y_ticks = 10;
        for (int i = 0; i < num_y_ticks; ++i) {
            int ty = plot_y + i * (plot_h / (num_y_ticks - 1.0f));
            fl_line(plot_x - 5, ty, plot_x, ty);
            fl_draw(y_ticks[i], plot_x - 28, ty + 4);
        }

        // Labels
        fl_draw("Emission Wavelength (nm)", plot_x + plot_w/2 - 60, plot_y + plot_h + 30);
        
        // Y-axis label (vertical) 
        // FLTK doesn't have rotated text easily, so we just write it horizontally for mock or simulate
        // We will just do a standard label on the left using a trick or normal text
        fl_font(FL_HELVETICA, 12);
        fl_draw("Excitation", x() + 5, plot_y + plot_h/2 - 10);
        fl_draw("Wavelength", x() + 5, plot_y + plot_h/2 + 5);
        fl_draw("(nm)", x() + 15, plot_y + plot_h/2 + 20);

        // Draw Colorbar
        int cb_x = plot_x + plot_w + 10;
        int cb_y = plot_y;
        int cb_w = 20;
        int cb_h = plot_h;
        
        // Draw gradient (Mocked with multiple colored rects)
        for (int i = 0; i < cb_h; ++i) {
            float t = (float)i / cb_h;
            // Simple fake rainbow: Red -> Yellow -> Green -> Blue
            int r = (t < 0.33f) ? 255 : (t < 0.66f) ? 255 * (1.0f - (t-0.33f)*3.0f) : 0;
            int g = (t < 0.33f) ? 255 * (t*3.0f) : (t < 0.66f) ? 255 : 255 * (1.0f - (t-0.66f)*3.0f);
            int b = (t < 0.33f) ? 0 : (t < 0.66f) ? 0 : 255 * ((t-0.66f)*3.0f);
            fl_color(fl_rgb_color(r, g, b));
            fl_line(cb_x, cb_y + i, cb_x + cb_w, cb_y + i);
        }
        fl_color(FL_BLACK);
        fl_rect(cb_x, cb_y, cb_w, cb_h); // Border
        
        fl_font(FL_HELVETICA, 11);
        fl_draw("-262", cb_x + cb_w + 5, cb_y + 10);
        fl_draw("--254", cb_x + cb_w + 5, cb_y + cb_h);
        fl_draw("Amplitude", cb_x + cb_w + 5, cb_y + cb_h/2);
    }
};

class TabPLEM : public Fl_Group {
public:
    TabPLEM(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int cx = X + 20;
        int cy = Y + 20;

        // -- File Name & Acquisition finished
        Fl_Box* lbl_file = new Fl_Box(cx, cy, 80, 15, "File Name");
        lbl_file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_file = new Fl_Input(cx, cy + 15, 80, 25);
        inp_file->value("map");

        LedIndicator* led_acq = new LedIndicator(cx + 100, cy + 10, 100, 25, "PLE Acquisition\nfinished?");

        // -- Start PLE Scan / STOP PLE
        cy += 50;
        LedButton* btn_start_ple = new LedButton(cx, cy, 120, 25, "Start PLE Scan");
        Fl_Button* btn_stop_ple = new Fl_Button(cx + 135, cy, 80, 25, "STOP PLE");
        btn_stop_ple->labelcolor(FL_RED);
        btn_stop_ple->box(FL_UP_BOX);

        // -- Acquisition Status
        cy += 35;
        Fl_Box* lbl_acq = new Fl_Box(cx, cy, 120, 15, "Acquisition Status");
        lbl_acq->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Output* out_acq = new Fl_Output(cx, cy + 15, 180, 25);
        out_acq->value("DRV_NOT_INITIALIZED");
        Fl_Int_Input* inp_acq_code = new Fl_Int_Input(cx + 190, cy + 15, 30, 25);
        inp_acq_code->value("75");
        inp_acq_code->readonly(1);

        // -- SuperK Varia (VIS) Group
        cy += 50;
        Fl_Group* grp_superk = new Fl_Group(cx, cy + 15, 230, 140);
        grp_superk->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_superk = new Fl_Box(cx, cy, 110, 15, "SuperK Varia (VIS)");
        lbl_superk->box(FL_ENGRAVED_FRAME); // Mocking a tab tab-header style
        
        Fl_Group* grp_ple_wl = new Fl_Group(cx + 10, cy + 35, 210, 110);
        grp_ple_wl->box(FL_ENGRAVED_BOX);
        Fl_Box* lbl_ple_wl = new Fl_Box(cx + 10, cy + 20, 100, 15, "PLE Wavelength");
        lbl_ple_wl->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Box* lbl_cwl = new Fl_Box(cx + 20, cy + 40, 150, 15, "Central Wavelength (nm)");
        lbl_cwl->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        // min / Bandpass
        Fl_Box* lbl_min = new Fl_Box(cx + 40, cy + 60, 50, 15, "min");
        Fl_Int_Input* inp_min = new Fl_Int_Input(cx + 40, cy + 75, 60, 25);
        inp_min->value("550");

        Fl_Box* lbl_bp = new Fl_Box(cx + 120, cy + 60, 60, 15, "Bandpass");
        Fl_Int_Input* inp_bp = new Fl_Int_Input(cx + 120, cy + 75, 60, 25);
        inp_bp->value("25");

        // max / Step Size
        Fl_Box* lbl_max = new Fl_Box(cx + 40, cy + 105, 50, 15, "max");
        Fl_Int_Input* inp_max = new Fl_Int_Input(cx + 40, cy + 120, 60, 25);
        inp_max->value("750");

        Fl_Box* lbl_step = new Fl_Box(cx + 120, cy + 105, 60, 15, "Step Size");
        Fl_Int_Input* inp_step = new Fl_Int_Input(cx + 120, cy + 120, 60, 25);
        inp_step->value("10");

        grp_ple_wl->end();
        grp_superk->end();

        // -- Bottom Stats
        cy += 165;
        Fl_Box* lbl_num_scan = new Fl_Box(cx, cy, 100, 15, "Number of Scans");
        lbl_num_scan->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* inp_num_scan = new Fl_Int_Input(cx, cy + 15, 60, 25);
        inp_num_scan->value("21");

        Fl_Box* lbl_pres_scan = new Fl_Box(cx + 110, cy, 100, 15, "Present Scan");
        lbl_pres_scan->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_pres_scan = new Fl_Input(cx + 110, cy + 15, 60, 25);
        inp_pres_scan->readonly(1);

        cy += 45;
        Fl_Box* lbl_prog = new Fl_Box(cx, cy, 80, 15, "Progress");
        lbl_prog->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_prog = new Fl_Input(cx, cy + 15, 60, 25);
        inp_prog->value("0.0%");
        inp_prog->readonly(1);

        Fl_Box* lbl_elapsed = new Fl_Box(cx + 110, cy, 100, 15, "Elapsed Time (s)");
        lbl_elapsed->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_elapsed = new Fl_Float_Input(cx + 110, cy + 15, 70, 25);
        inp_elapsed->value("0.00");
        inp_elapsed->readonly(1);

        Fl_Box* lbl_loop = new Fl_Box(cx + 250, cy, 100, 15, "PLE loop count");
        lbl_loop->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* inp_loop = new Fl_Int_Input(cx + 250, cy + 15, 50, 25);
        inp_loop->value("0");
        inp_loop->readonly(1);

        // -- PLEM Color Graph
        cy += 50;
        int gw = 400; // Graph Width
        int gh = 280; // Graph Height
        // If there's enough space, center it or put it below
        PLEMColorGraph* graph = new PLEMColorGraph(cx, cy, gw, gh, "");

        end();
    }
};
