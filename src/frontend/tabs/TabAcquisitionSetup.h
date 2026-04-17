#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Display.H>
#include "LedIndicator.h"

class TabAcquisitionSetup : public Fl_Group {
    LedIndicator* led_acquiring;
    LedIndicator* led_background;
    LedIndicator* led_ready;
    LedIndicator* led_finished;
    
    Fl_Input* input_next_action;
    
    Fl_Group* group_code;
    LedIndicator* led_code;
    Fl_Input* input_code_status;
    Fl_Text_Display* display_code;
    
    Fl_Box* progress_bar;
public:
    TabAcquisitionSetup(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int x_offset = X + 20;
        int y_offset = Y + 20;
        int sp = 30; // spacing

        // Column 1
        led_acquiring = new LedIndicator(x_offset, y_offset, 100, 20, "acquiring");
        led_background = new LedIndicator(x_offset, y_offset + sp, 100, 20, "background");

        // Column 2
        x_offset += 120;
        led_ready = new LedIndicator(x_offset, y_offset, 100, 20, "ready");
        led_finished = new LedIndicator(x_offset, y_offset + sp, 100, 20, "finished");

        // Next Action
        x_offset += 100;
        Fl_Box* lbl_next = new Fl_Box(x_offset, y_offset, 80, 20, "next action");
        lbl_next->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        input_next_action = new Fl_Input(x_offset, y_offset + 20, 150, 25);
        input_next_action->value("noop");

        // Code Group
        x_offset += 170;
        group_code = new Fl_Group(x_offset, y_offset - 10, 350, 70, "code");
        group_code->box(FL_ENGRAVED_FRAME);
        group_code->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);
        
        led_code = new LedIndicator(x_offset + 10, y_offset + 25, 20, 20); // No label
        input_code_status = new Fl_Input(x_offset + 40, y_offset + 25, 60, 25);
        input_code_status->value("d 0");
        
        display_code = new Fl_Text_Display(x_offset + 110, y_offset + 10, 220, 50);
        Fl_Text_Buffer* text_buf = new Fl_Text_Buffer();
        display_code->buffer(text_buf);
        
        group_code->end();

        // Progress bar at bottom
        progress_bar = new Fl_Box(X + 20, Y + 110, W - 40, 15);
        progress_bar->box(FL_ENGRAVED_BOX);
        progress_bar->color(FL_BLUE); // Assuming blue progress indicator

        end();
    }
};
