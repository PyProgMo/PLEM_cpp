#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>

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
        Fl_Group* grp_nkt_input = new Fl_Group(X + 20, Y + 60, 200, 280);
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
        Fl_Group* grp_nkt_output = new Fl_Group(X + 240, Y + 60, 120, 280);
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

        // Middle indicators - stacked vertically
        Fl_Input* bp_input = new Fl_Input(X + 140, Y + 370, 80, 25, "lightsource-bp");
        bp_input->deactivate();
        bp_input->value("--");
        
        Fl_Box* led_lightsource_set = new Fl_Box(FL_OVAL_BOX, X + 250, Y + 370, 15, 15, "lightsource-set");
        led_lightsource_set->color(FL_DARK_GREEN);
        led_lightsource_set->align(FL_ALIGN_RIGHT);

        Fl_Box* led_velleman = new Fl_Box(FL_OVAL_BOX, X + 250, Y + 400, 15, 15, "velleman-power-status");
        led_velleman->color(FL_DARK_GREEN);
        led_velleman->align(FL_ALIGN_RIGHT);

        // Powermeter Status Group
        Fl_Group* grp_pm_status = new Fl_Group(X + 20, Y + 480, 150, 140);
        grp_pm_status->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_pm_status = new Fl_Box(X + 20, Y + 465, 150, 20, "Powermeter Status");
        lbl_pm_status->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_pm_status->labelfont(FL_HELVETICA_BOLD);
        
        Fl_Light_Button* btn_auto_zero = new Fl_Light_Button(X + 60, Y + 495, 70, 25, "Perform Auto Zero");
        btn_auto_zero->align(FL_ALIGN_TOP);
        btn_auto_zero->selection_color(FL_GREEN);
        
        Fl_Float_Input* pm_pow = new Fl_Float_Input(X + 35, Y + 540, 90, 25, "Power (" "\xCE\xBC" "W)");
        pm_pow->align(FL_ALIGN_TOP_LEFT);
        pm_pow->readonly(1);
        pm_pow->value("0.000000");
        
        Fl_Float_Input* pm_wl = new Fl_Float_Input(X + 35, Y + 585, 90, 25, "Wavelength (nm)");
        pm_wl->align(FL_ALIGN_TOP_LEFT);
        pm_wl->readonly(1);
        pm_wl->value("0");
        grp_pm_status->end();
        
        g_light->end();
        
        Fl_Group* g_spec = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Spectrograph");
        g_spec->box(FL_ENGRAVED_BOX);
        g_spec->begin();
        
        // Spectrograph Input Group (Left)
        Fl_Group* grp_spec_input = new Fl_Group(X + 20, Y + 50, 220, 260);
        grp_spec_input->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_spec_input = new Fl_Box(X + 20, Y + 35, 100, 20, "Spectrograph Input");
        lbl_spec_input->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_spec_input->labelfont(FL_HELVETICA_BOLD);
        
        // Slit Width (Float)
        Fl_Box* lbl_slit = new Fl_Box(X + 30, Y + 60, 100, 16, "Slit Width (µm)");
        Fl_Float_Input* inp_slit = new Fl_Float_Input(X + 30, Y + 78, 180, 25);
        inp_slit->value("250.0");
        
        // Grating (Combobox)
        Fl_Box* lbl_grating = new Fl_Box(X + 30, Y + 110, 100, 16, "Grating");
        Fl_Choice* ch_grating = new Fl_Choice(X + 30, Y + 128, 180, 25);
        ch_grating->add("1200 blz 300l mm");
        ch_grating->add("1200 blz 500l mm");
        ch_grating->add("600 blz 200l mm");
        ch_grating->value(0);
        
        // Central Wavelength (Float)
        Fl_Box* lbl_cwl = new Fl_Box(X + 30, Y + 160, 150, 16, "Central Wavelength (nm)");
        Fl_Float_Input* inp_cwl = new Fl_Float_Input(X + 30, Y + 178, 180, 25);
        inp_cwl->value("950.0");
        
        // Filter (Combobox)
        Fl_Box* lbl_filter = new Fl_Box(X + 30, Y + 210, 100, 16, "Filter");
        Fl_Choice* ch_filter = new Fl_Choice(X + 30, Y + 228, 180, 25);
        ch_filter->add("Empty");
        ch_filter->add("Longpass 900 nm");
        ch_filter->add("Bandpass 950 nm");
        ch_filter->value(0);
        
        // Shutter (Combobox)
        Fl_Box* lbl_shutter = new Fl_Box(X + 30, Y + 260, 100, 16, "Shutter");
        Fl_Choice* ch_shutter = new Fl_Choice(X + 30, Y + 278, 180, 25);
        ch_shutter->add("Open");
        ch_shutter->add("Closed");
        ch_shutter->value(0);
        grp_spec_input->end();

        // Spectrograph Output Group (Right)
        Fl_Group* grp_spec_output = new Fl_Group(X + 260, Y + 50, 220, 260);
        grp_spec_output->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_spec_output = new Fl_Box(X + 260, Y + 35, 100, 20, "Spectrograph Output");
        lbl_spec_output->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_spec_output->labelfont(FL_HELVETICA_BOLD);
        
        // Slit Width (Read-only)
        Fl_Box* lbl_slit_out = new Fl_Box(X + 270, Y + 60, 120, 16, "Slit Width (µm)");
        Fl_Float_Input* out_slit = new Fl_Float_Input(X + 270, Y + 78, 150, 25);
        out_slit->readonly(1);
        out_slit->value("50.0");
        Fl_Box* led_slit = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 82, 12, 12, "");
        led_slit->color(FL_RED);
        
        // Grating (Read-only)
        Fl_Box* lbl_grating_out = new Fl_Box(X + 270, Y + 110, 120, 16, "Grating");
        Fl_Input* out_grating = new Fl_Input(X + 270, Y + 128, 150, 25);
        out_grating->readonly(1);
        out_grating->value("1250 blz 150l/mm");
        Fl_Box* led_grating = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 132, 12, 12, "");
        led_grating->color(FL_RED);
        
        // Central Wavelength (Read-only)
        Fl_Box* lbl_cwl_out = new Fl_Box(X + 270, Y + 160, 150, 16, "Central Wavelength (nm)");
        Fl_Float_Input* out_cwl = new Fl_Float_Input(X + 270, Y + 178, 150, 25);
        out_cwl->readonly(1);
        out_cwl->value("1000.00");
        Fl_Box* led_cwl = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 182, 12, 12, "");
        led_cwl->color(FL_RED);
        
        // Filter (Read-only)
        Fl_Box* lbl_filter_out = new Fl_Box(X + 270, Y + 210, 120, 16, "Filter");
        Fl_Input* out_filter = new Fl_Input(X + 270, Y + 228, 150, 25);
        out_filter->readonly(1);
        out_filter->value("Longpass 900 nm");
        Fl_Box* led_filter = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 232, 12, 12, "");
        led_filter->color(FL_RED);
        
        // Shutter (Read-only)
        Fl_Box* lbl_shutter_out = new Fl_Box(X + 270, Y + 260, 120, 16, "Shutter");
        Fl_Input* out_shutter = new Fl_Input(X + 270, Y + 278, 150, 25);
        out_shutter->readonly(1);
        out_shutter->value("Closed");
        Fl_Box* led_shutter = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 282, 12, 12, "");
        led_shutter->color(FL_RED);
        grp_spec_output->end();

        // Wavelength Limits Group
        Fl_Box* lbl_wl_limits = new Fl_Box(X + 20, Y + 330, 200, 20, "wavelength-limits");
        lbl_wl_limits->labelfont(FL_HELVETICA_BOLD);
        
        Fl_Group* grp_wl_limits = new Fl_Group(X + 20, Y + 355, 460, 80);
        grp_wl_limits->box(FL_ENGRAVED_FRAME);
        
        Fl_Box* lbl_wl_start = new Fl_Box(X + 50, Y + 365, 80, 16, "wl-start");
        Fl_Int_Input* inp_wl_start = new Fl_Int_Input(X + 50, Y + 383, 90, 25);
        inp_wl_start->value("0");
        
        Fl_Box* lbl_wl_step = new Fl_Box(X + 200, Y + 365, 80, 16, "wl-step");
        Fl_Int_Input* inp_wl_step = new Fl_Int_Input(X + 200, Y + 383, 90, 25);
        inp_wl_step->value("0");
        
        Fl_Box* lbl_wl_end = new Fl_Box(X + 350, Y + 365, 80, 16, "wl-end");
        Fl_Int_Input* inp_wl_end = new Fl_Int_Input(X + 350, Y + 383, 90, 25);
        inp_wl_end->value("0");
        grp_wl_limits->end();

        // Spectrograph Reset Group
        Fl_Box* lbl_spec_reset = new Fl_Box(X + 20, Y + 455, 200, 20, "spectrograph reset");
        lbl_spec_reset->labelfont(FL_HELVETICA_BOLD);
        
        Fl_Group* grp_spec_reset = new Fl_Group(X + 20, Y + 480, 200, 100);
        grp_spec_reset->box(FL_ENGRAVED_FRAME);
        
        Fl_Light_Button* tgl_grating_reset = new Fl_Light_Button(X + 90, Y + 495, 60, 25, "grating");
        tgl_grating_reset->selection_color(fl_rgb_color(255, 204, 0));
        Fl_Box* led_grating_reset = new Fl_Box(FL_OVAL_BOX, X + 160, Y + 499, 12, 12, "");
        led_grating_reset->color(FL_RED);
        
        Fl_Light_Button* tgl_slit_reset = new Fl_Light_Button(X + 90, Y + 530, 60, 25, "slit");
        tgl_slit_reset->selection_color(fl_rgb_color(255, 204, 0));
        Fl_Box* led_slit_reset = new Fl_Box(FL_OVAL_BOX, X + 160, Y + 534, 12, 12, "");
        led_slit_reset->color(FL_RED);
        grp_spec_reset->end();

        g_spec->end();
        
        Fl_Group* g_cam = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Camera");
        g_cam->box(FL_ENGRAVED_BOX);
        g_cam->begin();

        Fl_Box* lbl_camera = new Fl_Box(X + 30, Y + 55, 120, 16, "Camera");
        lbl_camera->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_camera = new Fl_Choice(X + 30, Y + 73, 180, 25);
        ch_camera->add("Andor");
        ch_camera->add("Xenics");
        ch_camera->value(0);

        Fl_Box* lbl_exp_time = new Fl_Box(X + 240, Y + 55, 180, 16, "Exposure Time (s)");
        lbl_exp_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_exp_time = new Fl_Float_Input(X + 240, Y + 73, 180, 25);
        inp_exp_time->value("0.1");

        Fl_Check_Button* chk_vertical_binning = new Fl_Check_Button(X + 30, Y + 126, 180, 25, "Full Vertical binning");
        chk_vertical_binning->value(0);

        Fl_Group* grp_andor = new Fl_Group(X + 30, Y + 170, 390, 95);
        grp_andor->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_andor = new Fl_Box(X + 40, Y + 152, 120, 16, "Andor settings");
        lbl_andor->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_andor = new Fl_Input(X + 45, Y + 198, 360, 25, "Config");
        inp_andor->value("default");
        grp_andor->end();

        Fl_Group* grp_xenics = new Fl_Group(X + 30, Y + 285, 390, 95);
        grp_xenics->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_xenics = new Fl_Box(X + 40, Y + 267, 120, 16, "Xenics settings");
        lbl_xenics->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_xenics = new Fl_Input(X + 45, Y + 313, 360, 25, "Config");
        inp_xenics->value("default");
        grp_xenics->end();

        Fl_Group* grp_andor_settings = new Fl_Group(X + 30, Y + 400, 390, 95);
        grp_andor_settings->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_andor_settings = new Fl_Box(X + 40, Y + 382, 140, 16, "Andor");
        lbl_andor_settings->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_andor_settings = new Fl_Input(X + 45, Y + 428, 360, 25, "Status");
        inp_andor_settings->value("ready");
        grp_andor_settings->end();
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
