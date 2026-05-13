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
#include <FL/Fl_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/fl_draw.H>
#include "LedButton.h"
#include "../../fbconnector/FBConnector.h"
#include <string>
#include "../../backend/AndorBackend.h"

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
    TabConfiguration(int X, int Y, int W, int H, const char* L = 0, AndorBackend* andor = nullptr) : Fl_Group(X, Y, W, H, L), m_andor(andor) {
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
        inp_slit = new Fl_Float_Input(X + 30, Y + 78, 180, 25);
        inp_slit->value("250.0");
        inp_slit->callback(cb_set_slit, this);
        inp_slit->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
        
        // Grating (Combobox)
        Fl_Box* lbl_grating = new Fl_Box(X + 30, Y + 110, 100, 16, "Grating");
        ch_grating = new Fl_Choice(X + 30, Y + 128, 180, 25);
        ch_grating->add("1200 blz 300l mm");
        ch_grating->add("1200 blz 500l mm");
        ch_grating->add("600 blz 200l mm");
        ch_grating->value(0);
        ch_grating->callback(cb_set_grating, this);
        
        // Central Wavelength (Float)
        Fl_Box* lbl_cwl = new Fl_Box(X + 30, Y + 160, 150, 16, "Central Wavelength (nm)");
        inp_cwl = new Fl_Float_Input(X + 30, Y + 178, 180, 25);
        inp_cwl->value("950.0");
        inp_cwl->callback(cb_set_cwl, this);
        inp_cwl->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
        
        // Filter (Combobox)
        Fl_Box* lbl_filter = new Fl_Box(X + 30, Y + 210, 100, 16, "Filter");
        ch_filter = new Fl_Choice(X + 30, Y + 228, 180, 25);
        ch_filter->add("Empty");
        ch_filter->add("Longpass 900 nm");
        ch_filter->add("Bandpass 950 nm");
        ch_filter->value(0);
        ch_filter->callback(cb_set_filter, this);
        
        // Shutter (Combobox)
        Fl_Box* lbl_shutter = new Fl_Box(X + 30, Y + 260, 100, 16, "Shutter");
        ch_shutter = new Fl_Choice(X + 30, Y + 278, 180, 25);
        ch_shutter->add("Open");
        ch_shutter->add("Closed");
        ch_shutter->value(0);
        ch_shutter->callback(cb_set_shutter, this);
        grp_spec_input->end();

        // Spectrograph Output Group (Right)
        Fl_Group* grp_spec_output = new Fl_Group(X + 260, Y + 50, 220, 260);
        grp_spec_output->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_spec_output = new Fl_Box(X + 260, Y + 35, 100, 20, "Spectrograph Output");
        lbl_spec_output->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_spec_output->labelfont(FL_HELVETICA_BOLD);
        
        // Slit Width (Read-only)
        Fl_Box* lbl_slit_out = new Fl_Box(X + 270, Y + 60, 120, 16, "Slit Width (µm)");
        out_slit = new Fl_Float_Input(X + 270, Y + 78, 150, 25);
        out_slit->readonly(1);
        out_slit->value("50.0");
        led_slit = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 82, 12, 12, "");
        led_slit->color(FL_RED);
        
        // Grating (Read-only)
        Fl_Box* lbl_grating_out = new Fl_Box(X + 270, Y + 110, 120, 16, "Grating");
        out_grating = new Fl_Input(X + 270, Y + 128, 150, 25);
        out_grating->readonly(1);
        out_grating->value("1250 blz 150l/mm");
        led_grating = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 132, 12, 12, "");
        led_grating->color(FL_RED);
        
        // Central Wavelength (Read-only)
        Fl_Box* lbl_cwl_out = new Fl_Box(X + 270, Y + 160, 150, 16, "Central Wavelength (nm)");
        out_cwl = new Fl_Float_Input(X + 270, Y + 178, 150, 25);
        out_cwl->readonly(1);
        out_cwl->value("1000.00");
        led_cwl = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 182, 12, 12, "");
        led_cwl->color(FL_RED);
        
        // Filter (Read-only)
        Fl_Box* lbl_filter_out = new Fl_Box(X + 270, Y + 210, 120, 16, "Filter");
        out_filter = new Fl_Input(X + 270, Y + 228, 150, 25);
        out_filter->readonly(1);
        out_filter->value("Longpass 900 nm");
        led_filter = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 232, 12, 12, "");
        led_filter->color(FL_RED);
        
        // Shutter (Read-only)
        Fl_Box* lbl_shutter_out = new Fl_Box(X + 270, Y + 260, 120, 16, "Shutter");
        out_shutter = new Fl_Input(X + 270, Y + 278, 150, 25);
        out_shutter->readonly(1);
        out_shutter->value("Closed");
        led_shutter = new Fl_Box(FL_OVAL_BOX, X + 430, Y + 282, 12, 12, "");
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
        
        tgl_grating_reset = new Fl_Light_Button(X + 90, Y + 495, 60, 25, "grating");
        tgl_grating_reset->selection_color(fl_rgb_color(255, 204, 0));
        tgl_grating_reset->callback(cb_reset_grating, this);
        led_grating_reset = new Fl_Box(FL_OVAL_BOX, X + 160, Y + 499, 12, 12, "");
        led_grating_reset->color(FL_RED);
        
        tgl_slit_reset = new Fl_Light_Button(X + 90, Y + 530, 60, 25, "slit");
        tgl_slit_reset->selection_color(fl_rgb_color(255, 204, 0));
        tgl_slit_reset->callback(cb_reset_slit, this);
        led_slit_reset = new Fl_Box(FL_OVAL_BOX, X + 160, Y + 534, 12, 12, "");
        led_slit_reset->color(FL_RED);
        grp_spec_reset->end();

        g_spec->end();
        
        Fl_Group* g_cam = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Camera");
        g_cam->box(FL_ENGRAVED_BOX);
        g_cam->begin();

        // Top Row: Camera
        int cx = X + 80;
        int cy = Y + 50;

        Fl_Box* lbl_cam_top = new Fl_Box(cx - 80, cy, 70, 25, "Camera");
        lbl_cam_top->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Input* inp_cam_newton = new Fl_Input(cx, cy, 100, 25);
        inp_cam_newton->value("Newton");
        Fl_Box* led_cam1 = new Fl_Box(FL_OVAL_BOX, cx + 120, cy + 2, 20, 20, "");
        led_cam1->color(FL_RED);
        Fl_Input* inp_cam_blank = new Fl_Input(cx + 160, cy, 100, 25);
        inp_cam_blank->value("");

        // Row 2: Exposure Time
        cy += 40;
        Fl_Box* lbl_exp = new Fl_Box(cx - 105, cy, 95, 25, "Exposure Time");
        lbl_exp->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_exp = new Fl_Float_Input(cx, cy, 100, 25);
        inp_exp->value("0.20");
        Fl_Box* led_cam2 = new Fl_Box(FL_OVAL_BOX, cx + 120, cy + 2, 20, 20, "");
        led_cam2->color(FL_RED);
        Fl_Float_Input* inp_exp2 = new Fl_Float_Input(cx + 160, cy, 100, 25);
        inp_exp2->value("0.00");

        // Row 3: Binning
        cy += 40;
        Fl_Button* btn_binning = new Fl_Button(cx - 30, cy, 130, 25, "Full Vertical Binning");
        btn_binning->box(FL_UP_BOX);
        Fl_Box* led_cam3 = new Fl_Box(FL_OVAL_BOX, cx + 120, cy + 2, 20, 20, "");
        led_cam3->color(FL_RED);

        // Sub Tabs
        cy += 45;
        DragAwareSubTabs* cam_tabs = new DragAwareSubTabs(X + 25, cy, W - 50, H - cy - 15);
        cam_tabs->begin();

        // Andor tab
        Fl_Group* t_andor = new Fl_Group(X + 25, cy + 25, W - 50, H - cy - 40, "Andor");
        t_andor->box(FL_ENGRAVED_BOX);
        t_andor->begin();

        int ax = X + 60;
        int ay = cy + 45;

        Fl_Box* lbl_hbin = new Fl_Box(ax, ay, 60, 25, "HBinning");
        lbl_hbin->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Input* inp_hbin = new Fl_Input(ax + 70, ay, 100, 25);
        inp_hbin->value("1x");
        Fl_Box* led_hbin = new Fl_Box(FL_OVAL_BOX, ax + 185, ay + 2, 20, 20, "");
        led_hbin->color(FL_RED);

        // Group 1 in Andor
        ay += 40;
        Fl_Group* grp_andor1 = new Fl_Group(ax - 20, ay, 280, 130);
        grp_andor1->box(FL_ENGRAVED_FRAME);
        grp_andor1->begin();

        int gy = ay + 15;
        Fl_Box* lbl_adchan = new Fl_Box(ax, gy, 60, 25, "ADchannel");
        lbl_adchan->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Spinner* spn_adchan = new Fl_Spinner(ax + 70, gy, 50, 25);
        spn_adchan->value(0);
        Fl_Box* led_adchan = new Fl_Box(FL_OVAL_BOX, ax + 140, gy + 2, 20, 20, "");
        led_adchan->color(FL_RED);

        gy += 35;
        Fl_Box* lbl_hss = new Fl_Box(ax, gy, 60, 25, "HSSpeed 2");
        lbl_hss->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Spinner* spn_hss = new Fl_Spinner(ax + 70, gy, 50, 25);
        spn_hss->value(0);
        Fl_Box* led_hss = new Fl_Box(FL_OVAL_BOX, ax + 140, gy + 2, 20, 20, "");
        led_hss->color(FL_RED);
        Fl_Input* inp_hss = new Fl_Input(ax + 175, gy, 40, 25);
        inp_hss->value("0");
        Fl_Box* lbl_hz = new Fl_Box(ax + 215, gy, 30, 25, "Mhz");
        lbl_hz->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        gy += 35;
        Fl_Box* lbl_pre = new Fl_Box(ax, gy, 60, 25, "pre-amp");
        lbl_pre->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Spinner* spn_pre = new Fl_Spinner(ax + 70, gy, 50, 25);
        spn_pre->value(0);
        Fl_Box* led_pre = new Fl_Box(FL_OVAL_BOX, ax + 140, gy + 2, 20, 20, "");
        led_pre->color(FL_RED);
        Fl_Input* inp_pre  = new Fl_Input(ax + 175, gy, 40, 25);
        inp_pre->value("0");
        Fl_Box* lbl_bx = new Fl_Box(ax + 215, gy, 20, 25, "x");
        lbl_bx->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        grp_andor1->end();

        // Group 2 in Andor
        ay += 140;
        Fl_Group* grp_andor2 = new Fl_Group(ax - 15, ay, 200, 50);
        grp_andor2->box(FL_ENGRAVED_FRAME);
        grp_andor2->begin();
        Fl_Box* lbl_center = new Fl_Box(ax-5, ay+12, 40, 25, "center");
        Fl_Input* inp_center = new Fl_Input(ax+40, ay+12, 40, 25);
        inp_center->value("102");
        Fl_Box* lbl_height = new Fl_Box(ax+90, ay+12, 40, 25, "height");
        Fl_Input* inp_height = new Fl_Input(ax+135, ay+12, 40, 25);
        inp_height->value("50");
        inp_height->deactivate();
        lbl_height->deactivate();
        grp_andor2->end();

        // Lower elements in Andor
        ay += 65;
        Fl_Box* lbl_idus = new Fl_Box(ax-10, ay, 120, 15, "iDus Type");
        lbl_idus->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_idus = new Fl_Choice(ax-10, ay+15, 170, 25);
        ch_idus->add("Standard EMCCD gain");
        ch_idus->value(0);
        Fl_Input* inp_idus2 = new Fl_Input(ax+170, ay+15, 30, 25);
        inp_idus2->value("0");

        Fl_Box* lbl_dll = new Fl_Box(ax+210, ay, 80, 15, "DLL error code");
        lbl_dll->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Input* inp_dll = new Fl_Input(ax+210, ay+15, 50, 25);
        inp_dll->value("0");

        t_andor->end();

        // Xenics tab
        Fl_Group* t_xenics = new Fl_Group(X + 25, cy + 25, W - 50, H - cy - 40, "Xenics");
        t_xenics->box(FL_ENGRAVED_BOX);
        t_xenics->hide();
        t_xenics->begin();

        int xx = X + 60;
        int xy = cy + 45;

        Fl_Box* lbl_x_width = new Fl_Box(xx, xy, 100, 25, "width");
        lbl_x_width->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Input* inp_x_width = new Fl_Input(xx + 110, xy, 100, 25);
        inp_x_width->value("320");
        
        xy += 35;
        Fl_Box* lbl_x_height = new Fl_Box(xx, xy, 100, 25, "height");
        lbl_x_height->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Input* inp_x_height = new Fl_Input(xx + 110, xy, 100, 25);
        inp_x_height->value("256");

        xy += 35;
        Fl_Box* lbl_x_time = new Fl_Box(xx, xy, 100, 25, "time");
        lbl_x_time->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_x_time = new Fl_Float_Input(xx + 110, xy, 100, 25);
        inp_x_time->value("0.05");

        xy += 35;
        Fl_Box* lbl_x_temp = new Fl_Box(xx, xy, 100, 25, "temperature");
        lbl_x_temp->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_x_temp = new Fl_Float_Input(xx + 110, xy, 100, 25);
        inp_x_temp->value("20.0");
        inp_x_temp->readonly(1);

        xy += 35;
        Fl_Box* lbl_x_tempset = new Fl_Box(xx - 20, xy, 120, 25, "temp-setpoint");
        lbl_x_tempset->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_x_tempset = new Fl_Float_Input(xx + 110, xy, 100, 25);
        inp_x_tempset->value("20.0");

        t_xenics->end();

        // Andor-settings tab
        Fl_Group* t_andor_set = new Fl_Group(X + 25, cy + 25, W - 50, H - cy - 40, "Andor-settings");
        t_andor_set->box(FL_ENGRAVED_BOX);
        t_andor_set->hide();
        t_andor_set->begin();

        int as_x = X + 60;
        int as_y = cy + 45;

        // Temperatures (Newton, iDus, Clara)
        Fl_Box* lbl_newton = new Fl_Box(as_x, as_y, 120, 25, "Newton Temp.");
        lbl_newton->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_newton_temp = new Fl_Float_Input(as_x + 130, as_y, 80, 25);
        inp_newton_temp->readonly(1);
        inp_newton_temp->value("0.0");

        as_y += 35;
        Fl_Box* lbl_idus_temp_lbl = new Fl_Box(as_x, as_y, 120, 25, "iDus Temp.");
        lbl_idus_temp_lbl->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_idus_temp = new Fl_Float_Input(as_x + 130, as_y, 80, 25);
        inp_idus_temp->readonly(1);
        inp_idus_temp->value("0.0");

        as_y += 35;
        Fl_Box* lbl_clara = new Fl_Box(as_x, as_y, 120, 25, "Clara Temp.");
        lbl_clara->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Float_Input* inp_clara_temp = new Fl_Float_Input(as_x + 130, as_y, 80, 25);
        inp_clara_temp->readonly(1);
        inp_clara_temp->value("0.0");

        // detector-warmup
        as_y += 45;
        Fl_Light_Button* btn_warmup = new Fl_Light_Button(as_x + 30, as_y, 150, 25, "detector-warmup");
        btn_warmup->selection_color(FL_GREEN);
        btn_warmup->value(1); // Set True as default

        t_andor_set->end();

        cam_tabs->end();

        g_cam->end();
        
        Fl_Group* g_stage = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Nanostage");
        g_stage->box(FL_ENGRAVED_BOX);
        g_stage->begin();

        Fl_Box* lbl_stage_title = new Fl_Box(X + 20, Y + 45, 350, 20, "Piezo Nanostage Control (Range: 0 - 300 " "\xC2\xB5" "m)");
        lbl_stage_title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_stage_title->labelfont(FL_HELVETICA_BOLD);

        // 1. Current Position
        Fl_Group* grp_stage_pos = new Fl_Group(X + 20, Y + 85, 150, 140);
        grp_stage_pos->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_stage_pos = new Fl_Box(X + 30, Y + 70, 120, 16, "Current Position");
        lbl_stage_pos->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_stage_pos->labelfont(FL_HELVETICA_BOLD);

        Fl_Box* lbl_pos_x = new Fl_Box(X + 30, Y + 100, 20, 25, "X:");
        Fl_Float_Input* out_pos_x = new Fl_Float_Input(X + 55, Y + 100, 70, 25);
        out_pos_x->readonly(1); out_pos_x->value("150.000");
        Fl_Box* lbl_unit_px = new Fl_Box(X + 130, Y + 100, 30, 25, "\xC2\xB5" "m");

        Fl_Box* lbl_pos_y = new Fl_Box(X + 30, Y + 135, 20, 25, "Y:");
        Fl_Float_Input* out_pos_y = new Fl_Float_Input(X + 55, Y + 135, 70, 25);
        out_pos_y->readonly(1); out_pos_y->value("150.000");
        Fl_Box* lbl_unit_py = new Fl_Box(X + 130, Y + 135, 30, 25, "\xC2\xB5" "m");

        Fl_Box* lbl_pos_z = new Fl_Box(X + 30, Y + 170, 20, 25, "Z:");
        Fl_Float_Input* out_pos_z = new Fl_Float_Input(X + 55, Y + 170, 70, 25);
        out_pos_z->readonly(1); out_pos_z->value("150.000");
        Fl_Box* lbl_unit_pz = new Fl_Box(X + 130, Y + 170, 30, 25, "\xC2\xB5" "m");
        grp_stage_pos->end();

        // 2. Absolute Movement
        Fl_Group* grp_stage_abs = new Fl_Group(X + 20, Y + 265, 150, 180);
        grp_stage_abs->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_stage_abs = new Fl_Box(X + 30, Y + 250, 120, 16, "Absolute Move");
        lbl_stage_abs->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_stage_abs->labelfont(FL_HELVETICA_BOLD);

        Fl_Box* lbl_abs_x = new Fl_Box(X + 30, Y + 280, 20, 25, "X:");
        Fl_Float_Input* inp_abs_x = new Fl_Float_Input(X + 55, Y + 280, 70, 25);
        inp_abs_x->value("150.000");
        Fl_Box* lbl_unit_ax = new Fl_Box(X + 130, Y + 280, 30, 25, "\xC2\xB5" "m");

        Fl_Box* lbl_abs_y = new Fl_Box(X + 30, Y + 315, 20, 25, "Y:");
        Fl_Float_Input* inp_abs_y = new Fl_Float_Input(X + 55, Y + 315, 70, 25);
        inp_abs_y->value("150.000");
        Fl_Box* lbl_unit_ay = new Fl_Box(X + 130, Y + 315, 30, 25, "\xC2\xB5" "m");

        Fl_Box* lbl_abs_z = new Fl_Box(X + 30, Y + 350, 20, 25, "Z:");
        Fl_Float_Input* inp_abs_z = new Fl_Float_Input(X + 55, Y + 350, 70, 25);
        inp_abs_z->value("150.000");
        Fl_Box* lbl_unit_az = new Fl_Box(X + 130, Y + 350, 30, 25, "\xC2\xB5" "m");

        Fl_Button* btn_move_abs = new Fl_Button(X + 45, Y + 395, 100, 30, "Go To Target");
        btn_move_abs->color(fl_rgb_color(220, 220, 220));
        grp_stage_abs->end();

        // 3. Step Sizes
        Fl_Group* grp_stage_steps = new Fl_Group(X + 190, Y + 85, 240, 140);
        grp_stage_steps->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_stage_steps = new Fl_Box(X + 200, Y + 70, 150, 16, "Step Sizes (" "\xC2\xB5" "m)");
        lbl_stage_steps->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_stage_steps->labelfont(FL_HELVETICA_BOLD);

        Fl_Box* lbl_step_x = new Fl_Box(X + 210, Y + 100, 20, 25, "X:");
        Fl_Float_Input* inp_step_x = new Fl_Float_Input(X + 240, Y + 100, 70, 25);
        inp_step_x->value("1.000");

        Fl_Box* lbl_step_y = new Fl_Box(X + 210, Y + 135, 20, 25, "Y:");
        Fl_Float_Input* inp_step_y = new Fl_Float_Input(X + 240, Y + 135, 70, 25);
        inp_step_y->value("1.000");

        Fl_Box* lbl_step_z = new Fl_Box(X + 210, Y + 170, 20, 25, "Z:");
        Fl_Float_Input* inp_step_z = new Fl_Float_Input(X + 240, Y + 170, 70, 25);
        inp_step_z->value("1.000");
        
        grp_stage_steps->end();

        // 4. Jog Controls (Relative Move)
        Fl_Group* grp_stage_jog = new Fl_Group(X + 190, Y + 235, 240, 210);
        grp_stage_jog->box(FL_ENGRAVED_FRAME);
        Fl_Box* lbl_stage_jog = new Fl_Box(X + 200, Y + 220, 150, 16, "Jog Controls");
        lbl_stage_jog->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_stage_jog->labelfont(FL_HELVETICA_BOLD);

        // XY Cross Layout
        Fl_Button* btn_jog_y_plus = new Fl_Button(X + 250, Y + 255, 40, 40, "+Y");
        Fl_Button* btn_jog_x_minus = new Fl_Button(X + 210, Y + 295, 40, 40, "-X");
        Fl_Button* btn_jog_x_plus = new Fl_Button(X + 290, Y + 295, 40, 40, "+X");
        Fl_Button* btn_jog_y_minus = new Fl_Button(X + 250, Y + 335, 40, 40, "-Y");

        // Z Axis Layout
        Fl_Button* btn_jog_z_plus = new Fl_Button(X + 360, Y + 255, 40, 40, "+Z");
        Fl_Button* btn_jog_z_minus = new Fl_Button(X + 360, Y + 335, 40, 40, "-Z");
        
        Fl_Button* btn_cancel_move = new Fl_Button(X + 220, Y + 395, 80, 30, "Cancel");
        btn_cancel_move->color(fl_rgb_color(220, 80, 80));
        
        Fl_Box* led_moving = new Fl_Box(FL_OVAL_BOX, X + 350, Y + 402, 16, 16, "moving");
        led_moving->color(FL_DARK_GREEN); // Default off
        led_moving->align(FL_ALIGN_RIGHT);
        
        grp_stage_jog->end();

        g_stage->end();
        
        Fl_Group* g_img = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "image");
        g_img->box(FL_ENGRAVED_BOX);
        g_img->begin();
        
        int ix = X + 80;
        int iy = Y + 80;
        
        // --- Magnification Section ---
        Fl_Box* lbl_mag = new Fl_Box(ix, iy, 120, 25, "magnification");
        lbl_mag->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        
        Fl_Float_Input* inp_mag = new Fl_Float_Input(ix + 130, iy, 70, 25);
        inp_mag->value("83.3333");
        
        Fl_Box* lbl_mag_x = new Fl_Box(ix + 205, iy, 15, 25, "x");
        lbl_mag_x->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        LedButton* btn_mag = new LedButton(ix + 230, iy, 120, 25, "magnification");

        // --- Image-Laser-Position Group ---
        iy += 60;
        Fl_Box* lbl_ilp = new Fl_Box(ix - 50, iy - 10, 160, 20, "image-laser-position");
        lbl_ilp->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        Fl_Group* grp_ilp = new Fl_Group(ix - 50, iy + 10, 250, 90);
        grp_ilp->box(FL_ENGRAVED_FRAME);
        grp_ilp->begin();
        
        int img_gx = ix - 40;
        int img_gy = iy + 25;
        
        Fl_Box* lbl_lx = new Fl_Box(img_gx, img_gy, 120, 25, "laser-x-position");
        lbl_lx->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Spinner* spn_lx = new Fl_Spinner(img_gx + 130, img_gy, 70, 25);
        spn_lx->value(520);
        spn_lx->maximum(10000);
        
        img_gy += 35;
        Fl_Box* lbl_ly = new Fl_Box(img_gx, img_gy, 120, 25, "laser-y-position");
        lbl_ly->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Spinner* spn_ly = new Fl_Spinner(img_gx + 130, img_gy, 70, 25);
        spn_ly->value(696);
        spn_ly->maximum(10000);
        
        grp_ilp->end();
        
        // Associated switches to the right of image-laser-position
        LedButton* btn_laser_zero = new LedButton(ix + 230, iy + 45, 120, 25, "laser-is-zero");
        LedButton* btn_glue = new LedButton(ix + 230, iy + 85, 150, 25, "glue scaling\nto image");

        // --- Pixel Scaling Group ---
        iy += 140;
        Fl_Box* lbl_ps = new Fl_Box(ix - 50, iy - 10, 120, 20, "Pixel Scaling");
        lbl_ps->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Group* grp_ps = new Fl_Group(ix - 50, iy + 10, 250, 250);
        grp_ps->box(FL_ENGRAVED_FRAME);
        grp_ps->begin();
        
        int px1 = ix - 40;
        int px2 = ix + 60;
        int py = iy + 20;
        
        // row 1: first x / first y labels
        Fl_Box* lbl_fx = new Fl_Box(px1, py, 90, 15, "first x");
        lbl_fx->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Box* lbl_fy = new Fl_Box(px2, py, 90, 15, "first y");
        lbl_fy->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        // row 1 inputs
        py += 15;
        Fl_Float_Input* inp_fx = new Fl_Float_Input(px1, py, 70, 25);
        inp_fx->value("498.805");
        Fl_Int_Input* inp_fy = new Fl_Int_Input(px2, py, 70, 25);
        inp_fy->value("0");
        
        // row 2: size x / size y labels
        py += 35;
        Fl_Box* lbl_sx = new Fl_Box(px1, py, 90, 15, "size x");
        lbl_sx->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Box* lbl_sy = new Fl_Box(px2, py, 90, 15, "size y");
        lbl_sy->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        // row 2 inputs
        py += 15;
        Fl_Int_Input* inp_sx = new Fl_Int_Input(px1, py, 70, 25);
        inp_sx->value("1024");
        Fl_Int_Input* inp_sy = new Fl_Int_Input(px2, py, 70, 25);
        inp_sy->value("1");

        // row 3: deltax / deltay labels
        py += 35;
        Fl_Box* lbl_dx = new Fl_Box(px1, py, 90, 15, "deltax");
        lbl_dx->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Box* lbl_dy = new Fl_Box(px2, py, 90, 15, "deltay");
        lbl_dy->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        // row 3 inputs
        py += 15;
        Fl_Float_Input* inp_dx = new Fl_Float_Input(px1, py, 70, 25);
        inp_dx->value("0.274766");
        Fl_Float_Input* inp_dy = new Fl_Float_Input(px2, py, 70, 25);
        inp_dy->value("256");

        // row 4: unit
        py += 45;
        Fl_Box* lbl_unit = new Fl_Box(px1, py, 40, 25, "unit");
        lbl_unit->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        Fl_Input* inp_unit = new Fl_Input(px1 + 45, py, 60, 25);
        inp_unit->value("nm");
        
        grp_ps->end();
        
        // Indicator to the right of Pixel Scaling
        Fl_Box* led_ps = new Fl_Box(FL_OVAL_BOX, ix + 230, iy + 120, 20, 20, "pixel scaling");
        led_ps->color(FL_GREEN);
        led_ps->align(FL_ALIGN_RIGHT);
        
        g_img->end();
        
        subtabs->end();
        end();
    }

private:
    AndorBackend* m_andor;

    // Spectrograph Inputs
    Fl_Float_Input* inp_slit;
    Fl_Choice* ch_grating;
    Fl_Float_Input* inp_cwl;
    Fl_Choice* ch_filter;
    Fl_Choice* ch_shutter;
    
    // Spectrograph Outputs
    Fl_Float_Input* out_slit;
    Fl_Box* led_slit;
    Fl_Input* out_grating;
    Fl_Box* led_grating;
    Fl_Float_Input* out_cwl;
    Fl_Box* led_cwl;
    Fl_Input* out_filter;
    Fl_Box* led_filter;
    Fl_Input* out_shutter;
    Fl_Box* led_shutter;
    
    // Resets
    Fl_Light_Button* tgl_grating_reset;
    Fl_Box* led_grating_reset;
    Fl_Light_Button* tgl_slit_reset;
    Fl_Box* led_slit_reset;

    static void cb_set_slit(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        float width = std::stof(tab->inp_slit->value());
        
        tab->led_slit->color(fl_rgb_color(255, 204, 0)); // yellow processing
        tab->led_slit->redraw();
        
        FBConnector::get().enqueueTask([tab, width]() {
            bool success = tab->m_andor->setSlitWidth(width);
            FBConnector::get().enqueueUIUpdate([tab, success, width]() {
                if (success) {
                    tab->out_slit->value(std::to_string(width).c_str());
                    tab->led_slit->color(FL_GREEN);
                } else {
                    tab->led_slit->color(FL_RED);
                }
                tab->led_slit->redraw();
            });
        });
    }

    static void cb_set_grating(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        int index = tab->ch_grating->value();
        std::string text = tab->ch_grating->text();
        
        tab->led_grating->color(fl_rgb_color(255, 204, 0));
        tab->led_grating->redraw();
        
        FBConnector::get().enqueueTask([tab, index, text]() {
            bool success = tab->m_andor->setGrating(index);
            FBConnector::get().enqueueUIUpdate([tab, success, text]() {
                if (success) {
                    tab->out_grating->value(text.c_str());
                    tab->led_grating->color(FL_GREEN);
                } else {
                    tab->led_grating->color(FL_RED);
                }
                tab->led_grating->redraw();
            });
        });
    }

    static void cb_set_cwl(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        float wl = std::stof(tab->inp_cwl->value());
        
        tab->led_cwl->color(fl_rgb_color(255, 204, 0));
        tab->led_cwl->redraw();
        
        FBConnector::get().enqueueTask([tab, wl]() {
            bool success = tab->m_andor->setCentralWavelength(wl);
            FBConnector::get().enqueueUIUpdate([tab, success, wl]() {
                if (success) {
                    tab->out_cwl->value(std::to_string(wl).c_str());
                    tab->led_cwl->color(FL_GREEN);
                } else {
                    tab->led_cwl->color(FL_RED);
                }
                tab->led_cwl->redraw();
            });
        });
    }

    static void cb_set_filter(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        int index = tab->ch_filter->value();
        std::string text = tab->ch_filter->text();
        
        tab->led_filter->color(fl_rgb_color(255, 204, 0));
        tab->led_filter->redraw();
        
        FBConnector::get().enqueueTask([tab, index, text]() {
            bool success = tab->m_andor->setFilter(index);
            FBConnector::get().enqueueUIUpdate([tab, success, text]() {
                if (success) {
                    tab->out_filter->value(text.c_str());
                    tab->led_filter->color(FL_GREEN);
                } else {
                    tab->led_filter->color(FL_RED);
                }
                tab->led_filter->redraw();
            });
        });
    }

    static void cb_set_shutter(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        int index = tab->ch_shutter->value();
        std::string text = tab->ch_shutter->text();
        
        tab->led_shutter->color(fl_rgb_color(255, 204, 0));
        tab->led_shutter->redraw();
        
        FBConnector::get().enqueueTask([tab, index, text]() {
            bool success = tab->m_andor->setShutter(index);
            FBConnector::get().enqueueUIUpdate([tab, success, text]() {
                if (success) {
                    tab->out_shutter->value(text.c_str());
                    tab->led_shutter->color(FL_GREEN);
                } else {
                    tab->led_shutter->color(FL_RED);
                }
                tab->led_shutter->redraw();
            });
        });
    }

    static void cb_reset_grating(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        
        tab->led_grating_reset->color(fl_rgb_color(255, 204, 0));
        tab->led_grating_reset->redraw();
        
        FBConnector::get().enqueueTask([tab]() {
            bool success = tab->m_andor->resetGrating();
            FBConnector::get().enqueueUIUpdate([tab, success]() {
                tab->tgl_grating_reset->value(0); // Turn block off
                if (success) {
                    tab->led_grating_reset->color(FL_GREEN);
                    tab->ch_grating->value(0);
                    tab->out_grating->value(tab->ch_grating->text());
                    tab->led_grating->color(FL_GREEN);
                    tab->led_grating->redraw();
                } else {
                    tab->led_grating_reset->color(FL_RED);
                }
                tab->led_grating_reset->redraw();
            });
        });
    }

    static void cb_reset_slit(Fl_Widget* w, void* data) {
        TabConfiguration* tab = static_cast<TabConfiguration*>(data);
        if (!tab->m_andor) return;
        
        tab->led_slit_reset->color(fl_rgb_color(255, 204, 0));
        tab->led_slit_reset->redraw();
        
        FBConnector::get().enqueueTask([tab]() {
            bool success = tab->m_andor->resetSlit();
            FBConnector::get().enqueueUIUpdate([tab, success]() {
                tab->tgl_slit_reset->value(0); // Turn block off
                if (success) {
                    tab->led_slit_reset->color(FL_GREEN);
                    tab->inp_slit->value("50.0");
                    tab->out_slit->value("50.0");
                    tab->led_slit->color(FL_GREEN);
                    tab->led_slit->redraw();
                } else {
                    tab->led_slit_reset->color(FL_RED);
                }
                tab->led_slit_reset->redraw();
            });
        });
    }
};
