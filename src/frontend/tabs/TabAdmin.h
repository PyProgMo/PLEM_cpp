#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Output.H>

class TabAdmin : public Fl_Group {
public:
    TabAdmin(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        int cx = X + 20;
        int cy = Y + 40; // Push down slightly for the group label

        Fl_Group* grp_error = new Fl_Group(cx, cy, 470, 460, "Error Display & Logging");
        grp_error->box(FL_ENGRAVED_FRAME);
        grp_error->align(FL_ALIGN_TOP_LEFT);
        grp_error->labelfont(FL_HELVETICA_BOLD);
        
        int gx = cx + 20;
        int gy = cy + 20;

        // Checkbox to log errors
        Fl_Check_Button* chk_log_errors = new Fl_Check_Button(gx, gy, 150, 25, "Log errors to database/file");
        
        gy += 40;
        
        // --- Current Error Display (Continuous) ---
        Fl_Box* lbl_current = new Fl_Box(gx, gy, 250, 20, "Current Error (Continuously Updating):");
        lbl_current->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_current->labelfont(FL_HELVETICA_BOLD);
        
        gy += 25;
        Fl_Text_Buffer* buf_current = new Fl_Text_Buffer();
        buf_current->text("System operational. No active errors.");
        Fl_Text_Display* out_current = new Fl_Text_Display(gx, gy, 430, 150); // 10 lines (~15px each) height
        out_current->buffer(buf_current);
        out_current->color(FL_LIGHT2);

        gy += 165;
        
        // --- Last Error Display ("Snapped") ---
        Fl_Box* lbl_last = new Fl_Box(gx, gy, 250, 20, "Last Error (Snapped):");
        lbl_last->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_last->labelfont(FL_HELVETICA_BOLD);
        
        // --- Timestamp ---
        Fl_Box* lbl_time = new Fl_Box(gx + 290, gy, 140, 20, "Timestamp:");
        lbl_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_time->labelfont(FL_HELVETICA_BOLD);

        gy += 25;
        Fl_Text_Buffer* buf_last = new Fl_Text_Buffer();
        buf_last->text("None.");
        Fl_Text_Display* out_last = new Fl_Text_Display(gx, gy, 280, 150);
        out_last->buffer(buf_last);
        out_last->color(FL_LIGHT2);

        Fl_Text_Buffer* buf_time = new Fl_Text_Buffer();
        buf_time->text("00:00:00.000\nDD.MM.YYYY");
        Fl_Text_Display* out_time = new Fl_Text_Display(gx + 290, gy, 140, 150);
        out_time->buffer(buf_time);
        out_time->color(FL_LIGHT2);

        grp_error->end();

        end();
    }
};
