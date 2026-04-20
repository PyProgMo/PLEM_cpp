#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Multiline_Output.H>
#include "LedButton.h"

class TabTimedMeasurement : public Fl_Group {
public:
    TabTimedMeasurement(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int ix = X + 30; // Moved left towards the start of the frame
        int iy = Y + 60; // Slightly higher up as well

        // Start Switch
        LedButton* btn_start = new LedButton(ix, iy, 80, 25, "start");

        // Timed-Timestamp
        Fl_Box* lbl_timestamp = new Fl_Box(ix + 120, iy - 20, 120, 20, "timed-timestamp");
        lbl_timestamp->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Multiline_Output* out_timestamp = new Fl_Multiline_Output(ix + 120, iy, 120, 40);
        out_timestamp->value("00:00:00.000\nDD.MM.YYYY");
        out_timestamp->color(FL_LIGHT2);
        
        // Wait / s
        iy += 80;
        Fl_Box* lbl_wait = new Fl_Box(ix, iy - 20, 80, 20, "wait / s");
        lbl_wait->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_wait = new Fl_Float_Input(ix, iy, 80, 25);
        inp_wait->value("0");

        // Waiting Progress
        Fl_Box* lbl_waiting = new Fl_Box(ix + 120, iy - 20, 80, 20, "waiting");
        lbl_waiting->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Progress* prog_waiting = new Fl_Progress(ix + 120, iy, 120, 20);
        prog_waiting->minimum(0);
        prog_waiting->maximum(100);
        prog_waiting->value(0);

        // Measurement type
        iy += 70;
        Fl_Box* lbl_meas_type = new Fl_Box(ix, iy - 20, 150, 20, "measurment type");
        lbl_meas_type->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_meas_type = new Fl_Choice(ix, iy, 120, 25);
        ch_meas_type->add("spectrum");
        ch_meas_type->value(0);

        // Timed-Counter
        iy += 90;
        Fl_Box* lbl_counter = new Fl_Box(ix, iy - 20, 120, 20, "timed-counter");
        lbl_counter->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Int_Input* inp_counter = new Fl_Int_Input(ix, iy, 80, 25);
        inp_counter->value("0");

        // Timed-Counter-Stop
        Fl_Box* lbl_counter_stop = new Fl_Box(ix + 150, iy - 20, 150, 20, "timed-counter-stop");
        lbl_counter_stop->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Spinner* spn_counter_stop = new Fl_Spinner(ix + 150, iy, 80, 25);
        spn_counter_stop->value(1);
        spn_counter_stop->minimum(1);
        spn_counter_stop->maximum(10000);

        // Shutdown Switch
        LedButton* btn_shutdown = new LedButton(ix + 310, iy, 100, 25, "shutdown");

        end();
    }
};
