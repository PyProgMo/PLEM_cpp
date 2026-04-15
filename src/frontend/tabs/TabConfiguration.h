#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Light_Button.H>

class DragAwareSubTabs : public Fl_Tabs {
public:
    DragAwareSubTabs(int X, int Y, int W, int H, const char* L = 0) : Fl_Tabs(X, Y, W, H, L) {}

    int handle(int event) override {
        int handled = Fl_Tabs::handle(event);
        if (event == FL_DRAG || event == FL_RELEASE) {
            refresh_tab_headers();
        }
        return handled;
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Tabs::resize(X, Y, W, H);
        refresh_tab_headers();
    }

private:
    void refresh_tab_headers() {
        for (int i = 0; i < children(); ++i) {
            if (Fl_Widget* tab = child(i)) {
                tab->redraw_label();
            }
        }
        redraw();
    }
};

class TabConfiguration : public Fl_Group {
public:
    TabConfiguration(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        DragAwareSubTabs* subtabs = new DragAwareSubTabs(X + 5, Y + 5, W - 10, H - 10);
        
        Fl_Group* g_light = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Lightsource");
        g_light->box(FL_ENGRAVED_BOX);
        g_light->begin();
        
        // NKT Input Group
        Fl_Group* grp_nkt_input = new Fl_Group(X + 20, Y + 60, 200, 250);
        grp_nkt_input->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_nkt_input = new Fl_Box(X + 20, Y + 45, 100, 20, "NKT Input");
        lbl_nkt_input->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_nkt_input->labelfont(FL_HELVETICA_BOLD);
        
        Fl_Light_Button* btn_em_in = new Fl_Light_Button(X + 130, Y + 70, 70, 25, "Emission");
        btn_em_in->align(FL_ALIGN_LEFT);
        btn_em_in->selection_color(FL_YELLOW);
        
        Fl_Light_Button* btn_mode_in = new Fl_Light_Button(X + 130, Y + 100, 70, 25, "Mode");
        btn_mode_in->align(FL_ALIGN_LEFT);
        btn_mode_in->selection_color(FL_YELLOW);
        
        Fl_Float_Input* inp_pow_lvl = new Fl_Float_Input(X + 130, Y + 135, 70, 25, "Power Level%");
        inp_pow_lvl->value("5.0");
        Fl_Float_Input* inp_fb_lvl = new Fl_Float_Input(X + 130, Y + 170, 70, 25, "Feedback Level%");
        inp_fb_lvl->value("0.0");
        Fl_Float_Input* inp_short_wl = new Fl_Float_Input(X + 130, Y + 205, 70, 25, "Short WL");
        inp_short_wl->value("560.0");
        Fl_Float_Input* inp_long_wl = new Fl_Float_Input(X + 130, Y + 240, 70, 25, "Long WL");
        inp_long_wl->value("580.0");
        Fl_Float_Input* inp_uv_wl = new Fl_Float_Input(X + 130, Y + 275, 70, 25, "UV WL");
        inp_uv_wl->value("400.0");
        grp_nkt_input->end();

        // NKT Output Group
        Fl_Group* grp_nkt_output = new Fl_Group(X + 240, Y + 60, 120, 250);
        grp_nkt_output->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_nkt_output = new Fl_Box(X + 240, Y + 45, 100, 20, "NKT Output");
        lbl_nkt_output->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_nkt_output->labelfont(FL_HELVETICA_BOLD);

        Fl_Box* led_em_out = new Fl_Box(FL_OVAL_BOX, X + 250, Y + 75, 15, 15, "Emission");
        led_em_out->color(FL_DARK_GREEN);
        led_em_out->align(FL_ALIGN_RIGHT);
        
        Fl_Input* out_text = new Fl_Input(X + 250, Y + 100, 100, 25);
        out_text->readonly(1);
        
        Fl_Float_Input* out_pow_lvl = new Fl_Float_Input(X + 250, Y + 135, 100, 25);
        out_pow_lvl->readonly(1);
        out_pow_lvl->value("0.0");
        
        Fl_Float_Input* out_fb_lvl = new Fl_Float_Input(X + 250, Y + 170, 100, 25);
        out_fb_lvl->readonly(1);
        out_fb_lvl->value("0.0");
        
        Fl_Float_Input* out_short_wl = new Fl_Float_Input(X + 250, Y + 205, 100, 25);
        out_short_wl->readonly(1);
        out_short_wl->value("0.0");
        
        Fl_Float_Input* out_long_wl = new Fl_Float_Input(X + 250, Y + 240, 100, 25);
        out_long_wl->readonly(1);
        out_long_wl->value("0.0");
        
        Fl_Float_Input* out_uv_wl = new Fl_Float_Input(X + 250, Y + 275, 100, 25);
        out_uv_wl->readonly(1);
        out_uv_wl->value("0.0");
        
        Fl_Float_Input* out_temp = new Fl_Float_Input(X + 250, Y + 315, 100, 25, "Temp (" "\xC2\xB0" "C)");
        out_temp->align(FL_ALIGN_TOP_LEFT);
        out_temp->readonly(1);
        out_temp->value("0.0");
        grp_nkt_output->end();

        // Middle indicators
        Fl_Input* bp_input = new Fl_Input(X + 140, Y + 350, 80, 25, "lightsource-bp");
        bp_input->deactivate();
        bp_input->value("--");
        
        Fl_Box* led_lightsource_set = new Fl_Box(FL_OVAL_BOX, X + 240, Y + 355, 15, 15, "lightsource-set");
        led_lightsource_set->color(FL_DARK_GREEN);
        led_lightsource_set->align(FL_ALIGN_RIGHT);

        Fl_Box* led_velleman = new Fl_Box(FL_OVAL_BOX, X + 240, Y + 385, 15, 15, "velleman-power-status");
        led_velleman->color(FL_DARK_GREEN);
        led_velleman->align(FL_ALIGN_RIGHT);

        // Powermeter Status Group
        Fl_Group* grp_pm_status = new Fl_Group(X + 20, Y + 460, 150, 140);
        grp_pm_status->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_pm_status = new Fl_Box(X + 20, Y + 445, 150, 20, "Powermeter Status");
        lbl_pm_status->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_pm_status->labelfont(FL_HELVETICA_BOLD);
        
        Fl_Light_Button* btn_auto_zero = new Fl_Light_Button(X + 60, Y + 475, 70, 25, "Perform Auto Zero");
        btn_auto_zero->align(FL_ALIGN_TOP);
        btn_auto_zero->selection_color(FL_GREEN);
        
        Fl_Float_Input* pm_pow = new Fl_Float_Input(X + 35, Y + 520, 90, 25, "Power (" "\xCE\xBC" "W)");
        pm_pow->align(FL_ALIGN_TOP_LEFT);
        pm_pow->readonly(1);
        pm_pow->value("0.000000");
        
        Fl_Float_Input* pm_wl = new Fl_Float_Input(X + 35, Y + 565, 90, 25, "Wavelength (nm)");
        pm_wl->align(FL_ALIGN_TOP_LEFT);
        pm_wl->readonly(1);
        pm_wl->value("0");
        grp_pm_status->end();
        
        g_light->end();
        
        Fl_Group* g_spec = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Spectrograph");
        g_spec->box(FL_ENGRAVED_BOX);
        g_spec->end();
        
        Fl_Group* g_cam = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Camera");
        g_cam->box(FL_ENGRAVED_BOX);
        g_cam->end();
        
        Fl_Group* g_stage = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Nanostage");
        g_stage->box(FL_ENGRAVED_BOX);
        g_stage->end();
        
        Fl_Group* g_img = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "image");
        g_img->box(FL_ENGRAVED_BOX);
        g_img->end();
        
        subtabs->end();
        end();
    }
};
