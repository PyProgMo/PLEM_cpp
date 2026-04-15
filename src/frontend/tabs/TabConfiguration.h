#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>

class TabConfiguration : public Fl_Group {
public:
    TabConfiguration(int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        Fl_Tabs* subtabs = new Fl_Tabs(X + 5, Y + 5, W - 10, H - 10);
        
        Fl_Group* g_light = new Fl_Group(X + 5, Y + 30, W - 10, H - 35, "Lightsource");
        g_light->box(FL_ENGRAVED_BOX);
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
