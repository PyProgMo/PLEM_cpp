#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Progress.H>
#include <FL/fl_draw.H>

// Custom Fancy Switch for this tab
class StageToggle : public Fl_Light_Button {
public:
    StageToggle(int x, int y, int w, int h, const char* l=0) : Fl_Light_Button(x,y,w,h,l) {
        selection_color(fl_rgb_color(255, 204, 0)); // yellow-orange
        box(FL_ROUND_UP_BOX); 
        down_box(FL_ROUND_DOWN_BOX);
        labelfont(FL_HELVETICA_BOLD);
    }
};

class TabAutoStage : public Fl_Group {
public:
    TabAutoStage(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int cx = X + W / 2;
        
        // -------------------------------------------------------------
        // TOP: Start / Save Toggles
        // -------------------------------------------------------------
        new StageToggle(cx - 90, Y + 15, 70, 25, "start");
        new StageToggle(cx + 20, Y + 15, 70, 25, "save");

        // -------------------------------------------------------------
        // CYCLE GROUP (Parameters)
        // -------------------------------------------------------------
        int cyc_w = 340;
        int cyc_h = 150;
        int cyc_x = cx - cyc_w / 2;
        int cyc_y = Y + 50;

        Fl_Group* grp_cycle = new Fl_Group(cyc_x, cyc_y, cyc_w, cyc_h);
        grp_cycle->box(FL_ROUNDED_BOX);
        grp_cycle->color(fl_rgb_color(240, 240, 240));

        Fl_Box* lbl_cycle = new Fl_Box(cyc_x, cyc_y + 5, cyc_w, 20, "Cycle");
        lbl_cycle->labelfont(FL_HELVETICA_BOLD);
        
        // Column Headers
        Fl_Box* c1 = new Fl_Box(cyc_x + 60, cyc_y + 30, 60, 30, "step\n[" "\xCE\xBC" "m]"); // micro symbol
        Fl_Box* c2 = new Fl_Box(cyc_x + 130, cyc_y + 30, 70, 30, "cycle mod");
        Fl_Box* c3 = new Fl_Box(cyc_x + 210, cyc_y + 30, 110, 30, "range\n[" "\xCE\xBC" "m]");
        c1->labelsize(11); c2->labelsize(11); c3->labelsize(11);

        const char* axisNames[] = {"x-axis", "y-axis", "z-axis"};
        const char* defStep[] = {"1", "1", "0"};
        const char* defCyc[] = {"1", "1", "0"};
        const char* defRangeMin[] = {"150", "150", "150"};
        const char* defRangeMax[] = {"150", "150", "150"};

        for (int i = 0; i < 3; i++) {
            int yy = cyc_y + 65 + i * 30;
            Fl_Box* aLbl = new Fl_Box(cyc_x + 10, yy, 50, 25, axisNames[i]);
            aLbl->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
            aLbl->labelsize(12);

            Fl_Float_Input* iStep = new Fl_Float_Input(cyc_x + 65, yy, 50, 25);
            iStep->value(defStep[i]);

            Fl_Int_Input* iMod = new Fl_Int_Input(cyc_x + 135, yy, 60, 25);
            iMod->value(defCyc[i]);

            Fl_Float_Input* iRMin = new Fl_Float_Input(cyc_x + 210, yy, 50, 25);
            iRMin->value(defRangeMin[i]);

            Fl_Float_Input* iRMax = new Fl_Float_Input(cyc_x + 270, yy, 50, 25);
            iRMax->value(defRangeMax[i]);
        }
        grp_cycle->end();

        // -------------------------------------------------------------
        // PROGRESS BAR
        // -------------------------------------------------------------
        Fl_Progress* prog = new Fl_Progress(cyc_x, cyc_y + cyc_h + 12, cyc_w, 10);
        prog->selection_color(FL_BLUE);
        prog->value(5); // Small progress shown in screenshot

        // -------------------------------------------------------------
        // MID SECTION: Load/Save/Measurement Type in one row
        // -------------------------------------------------------------
        int mid_y = cyc_y + cyc_h + 45;
        
        // Load / Save Toggle in one row
        new StageToggle(cx - 180, mid_y, 65, 25, "load");
        new StageToggle(cx - 105, mid_y, 65, 25, "save");

        // Measurement Type in same row
        Fl_Box* lbl_meas = new Fl_Box(cx - 20, mid_y, 100, 20, "measurement type");
        lbl_meas->labelsize(12); lbl_meas->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_meas = new Fl_Choice(cx - 20, mid_y + 18, 110, 25);
        ch_meas->add("spectrum");
        ch_meas->value(0);

        // Coordinates Section (moved down and to the left)
        int coord_y = mid_y + 65;
        Fl_Box* lbl_coord = new Fl_Box(cx - 180, coord_y, 100, 20, "coordinates");
        lbl_coord->labelsize(12);
        
        // Spinners (using Int_Input for simplicity)
        Fl_Int_Input* coord_s1 = new Fl_Int_Input(cx - 180, coord_y + 25, 65, 25);
        coord_s1->value("0");
        Fl_Int_Input* coord_s2 = new Fl_Int_Input(cx - 180, coord_y + 55, 65, 25);
        coord_s2->value("0");
        
        // 3x3 Table - moved left and down
        Fl_Group* coord_tbl = new Fl_Group(cx - 105, coord_y + 25, 140, 80);
        coord_tbl->box(FL_ENGRAVED_FRAME);
        const char* coordVals[3][3] = { {"0", "0", "0"}, {"0", "0", "0"}, {"0", "0", "0"} };
        for(int r=0; r<3; r++) {
            for(int c=0; c<3; c++) {
                Fl_Input* in = new Fl_Input(cx - 105 + c*45 + 2, coord_y + 25 + r*26 + 2, 41, 22);
                in->value(coordVals[r][c]);
            }
        }
        coord_tbl->end();

        // Excitation Section (moved down with more space)
        int exc_y = coord_y + 115;
        Fl_Box* lbl_exc = new Fl_Box(cx - 180, exc_y, 100, 20, "excitation");
        lbl_exc->labelsize(12);
        
        Fl_Int_Input* exc_s1 = new Fl_Int_Input(cx - 180, exc_y + 25, 65, 25);
        exc_s1->value("0");
        Fl_Int_Input* exc_s2 = new Fl_Int_Input(cx - 180, exc_y + 55, 65, 25);
        exc_s2->value("2195");
        
        // 2x3 Table - moved left with more space
        Fl_Group* exc_tbl = new Fl_Group(cx - 105, exc_y + 25, 140, 56);
        exc_tbl->box(FL_ENGRAVED_FRAME);
        for(int r=0; r<2; r++) {
            for(int c=0; c<3; c++) {
                Fl_Input* in = new Fl_Input(cx - 105 + c*45 + 2, exc_y + 25 + r*26 + 2, 41, 22);
                in->value("0");
                in->deactivate(); // Looks faded out in screenshot
            }
        }
        exc_tbl->end();

        // -------------------------------------------------------------
        // BOTTOM STATS
        // -------------------------------------------------------------
        int stat_y = exc_y + 92;
        
        Fl_Box* lbl_iter = new Fl_Box(cx - 150, stat_y, 70, 20, "iterator");
        lbl_iter->labelsize(12); lbl_iter->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* in_iter = new Fl_Int_Input(cx - 150, stat_y + 18, 80, 24);
        in_iter->value("0"); in_iter->readonly(1); in_iter->textsize(13);

        Fl_Box* lbl_tot = new Fl_Box(cx - 40, stat_y, 70, 20, "total");
        lbl_tot->labelsize(12); lbl_tot->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* in_tot = new Fl_Int_Input(cx - 40, stat_y + 18, 80, 24);
        in_tot->value("0"); in_tot->readonly(1); in_tot->textsize(13);

        Fl_Box* lbl_time = new Fl_Box(cx + 70, stat_y, 80, 20, "time/min");
        lbl_time->labelsize(12); lbl_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* in_time = new Fl_Int_Input(cx + 70, stat_y + 18, 80, 24);
        in_time->value("0"); in_time->readonly(1); in_time->textsize(13);

        // -------------------------------------------------------------
        // COORDINATE GRAPH PLACEHOLDER
        // -------------------------------------------------------------
        int g_y = stat_y + 48;
        Fl_Box* lbl_graph = new Fl_Box(cyc_x, g_y, 110, 20, "Coordinate Graph");
        lbl_graph->labelsize(12); lbl_graph->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Group* grp_graph = new Fl_Group(cyc_x, g_y + 20, cyc_w, 160);
        grp_graph->box(FL_ENGRAVED_FRAME);
        
        // Dummy block to represent graph area
        const int plot_x = cyc_x + 24;
        const int plot_y = g_y + 34;
        const int plot_w = cyc_w - 80;
        const int plot_h = 136;
        Fl_Box* plot_area = new Fl_Box(FL_FLAT_BOX, plot_x, plot_y, plot_w, plot_h, "");
        plot_area->color(FL_BLACK); // Graph is dark in the preview
        
        Fl_Box* axis_y = new Fl_Box(FL_FLAT_BOX, plot_x + 8, plot_y + 8, 2, plot_h - 16, "");
        axis_y->color(FL_WHITE);
        Fl_Box* axis_x = new Fl_Box(FL_FLAT_BOX, plot_x + 8, plot_y + plot_h - 10, plot_w - 16, 2, "");
        axis_x->color(FL_WHITE);
        Fl_Box* axis_y_lbl = new Fl_Box(plot_x - 16, plot_y + 4, 12, 20, "Y");
        axis_y_lbl->labelcolor(FL_WHITE);
        axis_y_lbl->labelsize(11);
        Fl_Box* axis_x_lbl = new Fl_Box(plot_x + plot_w - 10, plot_y + plot_h - 8, 12, 20, "X");
        axis_x_lbl->labelcolor(FL_WHITE);
        axis_x_lbl->labelsize(11);
        
        // Dummy legend
        Fl_Box* grad = new Fl_Box(FL_FLAT_BOX, cyc_x + cyc_w - 32, g_y + 44, 12, 120, "");
        grad->color(FL_BLUE); // Gradient placeholder

        grp_graph->end();

        end();
    }
};
