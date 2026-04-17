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
        int x_offset = X + 10;
        int y_offset = Y + 20;
        int sp = 30; // spacing

        // Column 1
        led_acquiring = new LedIndicator(x_offset, y_offset, 90, 20, "acquiring");
        led_background = new LedIndicator(x_offset, y_offset + sp, 90, 20, "background");

        // Column 2
        int col2_x = x_offset + 90;
        led_ready = new LedIndicator(col2_x, y_offset, 80, 20, "ready");
        led_finished = new LedIndicator(col2_x, y_offset + sp, 80, 20, "finished");

        // Next Action (moved closer to LEDs, made narrower)
        int col3_x = col2_x + 85;
        Fl_Box* lbl_next = new Fl_Box(col3_x, y_offset, 80, 20, "next action");
        lbl_next->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        input_next_action = new Fl_Input(col3_x, y_offset + 20, 100, 25); // narrower
        input_next_action->value("noop");

        // Code Group (starting immediately right after Next Action)
        int code_x = col3_x + 110;
        int code_w = W > (code_x + 400) ? (W - code_x - 10) : (W - code_x - 10);
        if (code_w < 300) code_w = 300; // minimum width
        
        group_code = new Fl_Group(code_x, y_offset - 10, code_w, 70, "code");
        group_code->box(FL_ENGRAVED_FRAME);
        group_code->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);
        
        led_code = new LedIndicator(code_x + 5, y_offset + 25, 20, 20); // No label
        input_code_status = new Fl_Input(code_x + 30, y_offset + 25, 40, 25);
        input_code_status->value("d 0");
        
        display_code = new Fl_Text_Display(code_x + 80, y_offset + 10, code_w - 90, 50);
        Fl_Text_Buffer* text_buf = new Fl_Text_Buffer();
        display_code->buffer(text_buf);
        
        group_code->end();

        // Progress bar at bottom
        progress_bar = new Fl_Box(X + 10, Y + 110, W - 20, 15);
        progress_bar->box(FL_ENGRAVED_BOX);
        progress_bar->color(FL_BLUE); // Assuming blue progress indicator

        end();
    }
};
