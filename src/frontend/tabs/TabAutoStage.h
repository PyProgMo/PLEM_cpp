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
        int cyc_h = 160;
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
        Fl_Progress* prog = new Fl_Progress(cyc_x, cyc_y + cyc_h + 15, cyc_w, 10);
        prog->selection_color(FL_BLUE);
        prog->value(5); // Small progress shown in screenshot

        // -------------------------------------------------------------
        // MID SECTION: Load/Save & Matrices
        // -------------------------------------------------------------
        int mid_y = cyc_y + cyc_h + 40;
        
        // Load / Save Toggle
        new StageToggle(cx - 150, mid_y, 70, 25, "load");
        new StageToggle(cx - 150, mid_y + 35, 70, 25, "save");

        // Coordinates Matrix
        Fl_Box* lbl_coord = new Fl_Box(cx + 40, mid_y - 20, 100, 20, "coordinates");
        lbl_coord->labelsize(12);
        
        // Spinners (using Int_Input for simplicity)
        Fl_Int_Input* coord_s1 = new Fl_Int_Input(cx - 30, mid_y, 70, 25);
        coord_s1->value("0");
        Fl_Int_Input* coord_s2 = new Fl_Int_Input(cx - 30, mid_y + 35, 70, 25);
        coord_s2->value("0");
        
        // 3x3 Table
        Fl_Group* coord_tbl = new Fl_Group(cx + 50, mid_y, 160, 85);
        coord_tbl->box(FL_ENGRAVED_FRAME);
        const char* coordVals[3][3] = { {"0", "0", "0"}, {"0", "0", "0"}, {"0", "0", "0"} };
        for(int r=0; r<3; r++) {
            for(int c=0; c<3; c++) {
                Fl_Input* in = new Fl_Input(cx + 50 + c*50 + 2, mid_y + r*28 + 2, 46, 24);
                in->value(coordVals[r][c]);
            }
        }
        coord_tbl->end();

        // Measurement Type
        int mid2_y = mid_y + 90;
        Fl_Box* lbl_meas = new Fl_Box(cx - 160, mid2_y, 110, 20, "measurement type");
        lbl_meas->labelsize(12); lbl_meas->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_meas = new Fl_Choice(cx - 160, mid2_y + 20, 110, 25);
        ch_meas->add("spectrum");
        ch_meas->value(0);

        // Excitation Matrix
        Fl_Box* lbl_exc = new Fl_Box(cx + 40, mid2_y, 100, 20, "excitation");
        lbl_exc->labelsize(12);
        
        Fl_Int_Input* exc_s1 = new Fl_Int_Input(cx - 30, mid2_y + 20, 70, 25);
        exc_s1->value("0");
        Fl_Int_Input* exc_s2 = new Fl_Int_Input(cx - 30, mid2_y + 55, 70, 25);
        exc_s2->value("2195");
        
        // 2x3 Table
        Fl_Group* exc_tbl = new Fl_Group(cx + 50, mid2_y + 20, 160, 60);
        exc_tbl->box(FL_ENGRAVED_FRAME);
        for(int r=0; r<2; r++) {
            for(int c=0; c<3; c++) {
                Fl_Input* in = new Fl_Input(cx + 50 + c*50 + 2, mid2_y + 20 + r*28 + 2, 46, 24);
                in->value("0");
                in->deactivate(); // Looks faded out in screenshot
            }
        }
        exc_tbl->end();

        // -------------------------------------------------------------
        // BOTTOM STATS
        // -------------------------------------------------------------
        int stat_y = mid2_y + 95;
        
        Fl_Box* lbl_iter = new Fl_Box(cx - 150, stat_y, 70, 20, "iterator");
        lbl_iter->labelsize(15); lbl_iter->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* in_iter = new Fl_Int_Input(cx - 150, stat_y + 20, 80, 30);
        in_iter->value("0"); in_iter->readonly(1); in_iter->textsize(15);

        Fl_Box* lbl_tot = new Fl_Box(cx - 40, stat_y, 70, 20, "total");
        lbl_tot->labelsize(15); lbl_tot->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* in_tot = new Fl_Int_Input(cx - 40, stat_y + 20, 80, 30);
        in_tot->value("0"); in_tot->readonly(1); in_tot->textsize(15);

        Fl_Box* lbl_time = new Fl_Box(cx + 70, stat_y, 80, 20, "time/min");
        lbl_time->labelsize(15); lbl_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* in_time = new Fl_Int_Input(cx + 70, stat_y + 20, 80, 30);
        in_time->value("0"); in_time->readonly(1); in_time->textsize(15);

        // -------------------------------------------------------------
        // COORDINATE GRAPH PLACEHOLDER
        // -------------------------------------------------------------
        int g_y = stat_y + 60;
        Fl_Box* lbl_graph = new Fl_Box(cyc_x, g_y, 110, 20, "Coordinate Graph");
        lbl_graph->labelsize(12); lbl_graph->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Group* grp_graph = new Fl_Group(cyc_x, g_y + 20, cyc_w, 180);
        grp_graph->box(FL_ENGRAVED_FRAME);
        
        // Dummy block to represent graph area
        Fl_Box* plot_area = new Fl_Box(FL_FLAT_BOX, cyc_x + 10, g_y + 30, cyc_w - 60, 160, "");
        plot_area->color(FL_BLACK); // Graph is dark in the preview
        
        // Dummy legend
        Fl_Box* grad = new Fl_Box(FL_FLAT_BOX, cyc_x + cyc_w - 40, g_y + 40, 15, 130, "");
        grad->color(FL_BLUE); // Gradient placeholder

        grp_graph->end();

        end();
    }
};
