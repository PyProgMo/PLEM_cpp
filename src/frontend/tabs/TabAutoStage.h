#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Progress.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include "../../fbconnector/FBConnector.h"
#include "../../../include/ErrorLogger.h"

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

class CoordinateGraph : public Fl_Widget {
    int num_ticks_x;
    int num_ticks_y;
    float start_x, end_x;
    float start_y, end_y;
    float cover_start_x, cover_end_x;
    float cover_start_y, cover_end_y;
public:
    CoordinateGraph(int X, int Y, int W, int H, const char* L=0) : Fl_Widget(X,Y,W,H,L) {
        num_ticks_x = 5;
        num_ticks_y = 5;
        start_x = 0.0f; end_x = 300.0f;
        start_y = 0.0f; end_y = 300.0f;
        cover_start_x = 50.0f; cover_end_x = 250.0f; // Mock coverage
        cover_start_y = 50.0f; cover_end_y = 250.0f;
    }
    void draw() override {
        // Draw background
        fl_color(FL_BLACK);
        fl_rectf(x(), y(), w(), h());
        
        int pad_l = 30; // padding left for Y ticks
        int pad_b = 20; // padding bottom for X ticks
        int plot_x = x() + pad_l;
        int plot_y = y() + 15;
        int plot_w = w() - pad_l - 15;
        int plot_h = h() - pad_b - 15;

        // Draw Mock Covered area to highlight stage coverage
        fl_color(fl_rgb_color(0, 60, 120)); // Dim blue for coverage highlight
        float cx1 = plot_x + (cover_start_x - start_x) / (end_x - start_x) * plot_w;
        float cw = (cover_end_x - cover_start_x) / (end_x - start_x) * plot_w;
        float ch = (cover_end_y - cover_start_y) / (end_y - start_y) * plot_h;
        float cy1 = plot_y + plot_h - ((cover_start_y - start_y) / (end_y - start_y) * plot_h) - ch;
        fl_rectf(cx1, cy1, cw, ch);

        fl_color(FL_WHITE);
        fl_line_style(FL_SOLID, 1);
        // Draw Axis
        fl_line(plot_x, plot_y - 5, plot_x, plot_y + plot_h); // Y axis Main line
        fl_line(plot_x, plot_y + plot_h, plot_x + plot_w + 5, plot_y + plot_h); // X axis Main line
        
        // Thin and small font
        fl_font(FL_HELVETICA, 10);
        
        // Draw X ticks (Start, End, and dynamic intermediates)
        for (int i = 0; i < num_ticks_x; ++i) {
            float t = (float)i / (num_ticks_x - 1);
            int tx = plot_x + t * plot_w;
            fl_line(tx, plot_y + plot_h, tx, plot_y + plot_h + 4); // Tick mark
            char buf[32];
            sprintf(buf, "%.0f", start_x + t * (end_x - start_x));
            fl_draw(buf, tx - 8, plot_y + plot_h + 14);
        }

        // Draw Y ticks (Start, End, and dynamic intermediates)
        for (int i = 0; i < num_ticks_y; ++i) {
            float t = (float)i / (num_ticks_y - 1);
            int ty = plot_y + plot_h - t * plot_h;
            fl_line(plot_x - 4, ty, plot_x, ty); // Tick mark
            char buf[32];
            sprintf(buf, "%.0f", start_y + t * (end_y - start_y));
            fl_draw(buf, x() + 5, ty + 3);
        }
        
        // Axis Labels
        fl_color(FL_LIGHT2);
        fl_draw("X", plot_x + plot_w - 5, plot_y + plot_h - 5);
        fl_draw("Y", x() + 15, plot_y - 5);
    }
};

class TabAutoStage : public Fl_Group {
private:
    // Internal coordinate storage (loaded from file or generated from cycle params)
    std::vector<std::vector<float>> internal_coordinates;
    
    // UI Elements we need to track
    Fl_Float_Input* cycle_step[3];
    Fl_Int_Input* cycle_mod[3];
    Fl_Float_Input* cycle_range_min[3];
    Fl_Float_Input* cycle_range_max[3];
    
    // Excitation display (live coordinates during motion)
    Fl_Input* excitation_display[2][3];
    
    // Coordinate viewer (shift through loaded coordinates)
    Fl_Int_Input* coord_spinner[2];  // For shifting (start coord index)
    Fl_Input* coord_display[3][3];   // 3x3 table showing 3 consecutive coordinates
    int coord_view_index;            // Current starting index for coordinate display
    
    // Statistics display
    Fl_Int_Input* stat_iterator;
    Fl_Int_Input* stat_total;
    Fl_Int_Input* stat_time;
    
    Fl_Progress* progress_bar;
    StageToggle* btn_start;
    
    // Autostage state
    bool is_running;
    std::chrono::steady_clock::time_point start_time;

    // Helper: Load coordinates from file
    bool loadCoordinatesFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ErrorLogger::GetInstance().LogError(
                ErrorCodes::CATEGORY_STAGE, 0x0001,
                "AutoStage: Failed to load coordinates", 
                "Could not open file: " + filepath
            );
            return false;
        }
        
        internal_coordinates.clear();
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::vector<float> coord;
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, ',')) {
                try {
                    coord.push_back(std::stof(token));
                } catch (...) {
                    ErrorLogger::GetInstance().LogError(
                        ErrorCodes::CATEGORY_STAGE, 0x0002,
                        "AutoStage: Invalid coordinate format in file",
                        "Line: " + line
                    );
                    return false;
                }
            }
            
            if (coord.size() == 3) {
                internal_coordinates.push_back(coord);
            }
        }
        
        file.close();
        
        // Update coordinate display to show first 3 coordinates
        coord_view_index = 0;
        updateCoordinateDisplay();
        
        return true;
    }

    // Helper: Save coordinates to file
    bool saveCoordinatesToFile(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            ErrorLogger::GetInstance().LogError(
                ErrorCodes::CATEGORY_STAGE, 0x0003,
                "AutoStage: Failed to save coordinates",
                "Could not open file: " + filepath
            );
            return false;
        }
        
        for (const auto& coord : internal_coordinates) {
            file << std::fixed << std::setprecision(2);
            file << coord[0] << "," << coord[1] << "," << coord[2] << "\n";
        }
        
        file.close();
        return true;
    }

    // Helper: Generate coordinates from cycle parameters
    void generateCoordinatesFromCycle() {
        internal_coordinates.clear();
        
        try {
            float step_x = std::stof(cycle_step[0]->value());
            float step_y = std::stof(cycle_step[1]->value());
            float step_z = std::stof(cycle_step[2]->value());
            
            int mod_x = std::stoi(cycle_mod[0]->value());
            int mod_y = std::stoi(cycle_mod[1]->value());
            int mod_z = std::stoi(cycle_mod[2]->value());
            
            float range_min_x = std::stof(cycle_range_min[0]->value());
            float range_max_x = std::stof(cycle_range_max[0]->value());
            float range_min_y = std::stof(cycle_range_min[1]->value());
            float range_max_y = std::stof(cycle_range_max[1]->value());
            float range_min_z = std::stof(cycle_range_min[2]->value());
            float range_max_z = std::stof(cycle_range_max[2]->value());
            
            // Generate all coordinate points
            for (float x = range_min_x; x <= range_max_x; x += step_x) {
                for (float y = range_min_y; y <= range_max_y; y += step_y) {
                    for (float z = range_min_z; z <= range_max_z; z += step_z) {
                        internal_coordinates.push_back({x, y, z});
                    }
                }
            }
            
            coord_view_index = 0;  // Reset viewer to first coordinate
            updateStatistics();
            updateCoordinateDisplay();
        } catch (...) {
            ErrorLogger::GetInstance().LogError(
                ErrorCodes::CATEGORY_STAGE, 0x0004,
                "AutoStage: Invalid cycle parameters"
            );
        }
    }

    // Helper: Update statistics display
    void updateStatistics() {
        int total = internal_coordinates.size();
        stat_total->value(std::to_string(total).c_str());
        
        // Estimate time based on dummy measurement time per coordinate (~10 seconds)
        int estimated_seconds = total * 10;
        int hours = estimated_seconds / 3600;
        int minutes = (estimated_seconds % 3600) / 60;
        int seconds = estimated_seconds % 60;
        
        std::string time_str = std::to_string(hours) + "h " + 
                               std::to_string(minutes) + "m " + 
                               std::to_string(seconds) + "s";
        stat_time->value(time_str.c_str());
    }

    // Helper: Update coordinate viewer display (shows 3 consecutive coordinates with 3 decimal places)
    void updateCoordinateDisplay() {
        for (int r = 0; r < 3; r++) {
            int coord_idx = coord_view_index + r;
            if (coord_idx >= 0 && coord_idx < (int)internal_coordinates.size()) {
                const auto& coord = internal_coordinates[coord_idx];
                for (int c = 0; c < 3; c++) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "%.3f", coord[c]);
                    coord_display[r][c]->value(buf);
                }
            } else {
                // Empty cells if index out of range
                for (int c = 0; c < 3; c++) {
                    coord_display[r][c]->value("");
                }
            }
        }
    }

    // Helper: Update excitation display (with 3 decimal places)
    void updateExcitationDisplay(int coord_index) {
        if (coord_index < 0 || coord_index >= (int)internal_coordinates.size()) {
            return;
        }
        
        const auto& coord = internal_coordinates[coord_index];
        for (int i = 0; i < 3; i++) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.3f", coord[i]);
            excitation_display[0][i]->value(buf);
        }
    }

    // Callback: Load coordinates from file
    static void loadCoordinatesCallback(Fl_Widget* w, void* data) {
        TabAutoStage* tab = static_cast<TabAutoStage*>(data);
        
        Fl_Native_File_Chooser chooser;
        chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
        chooser.filter("Text files (*.txt)\tAll files (*)");
        chooser.title("Load Coordinates");
        
        if (chooser.show() == 0) {
            std::string filepath = chooser.filename();
            if (tab->loadCoordinatesFromFile(filepath)) {
                tab->updateStatistics();
                ErrorLogger::GetInstance().LogError(
                    ErrorCodes::CATEGORY_STAGE, 0x1000,
                    "AutoStage: Coordinates loaded successfully",
                    "File: " + filepath
                );
            }
        }
    }

    // Callback: Save coordinates to file
    static void saveCoordinatesCallback(Fl_Widget* w, void* data) {
        TabAutoStage* tab = static_cast<TabAutoStage*>(data);
        
        Fl_Native_File_Chooser chooser;
        chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
        chooser.filter("Text files (*.txt)\tAll files (*)");
        chooser.title("Save Coordinates");
        
        if (chooser.show() == 0) {
            std::string filepath = chooser.filename();
            if (tab->saveCoordinatesToFile(filepath)) {
                ErrorLogger::GetInstance().LogError(
                    ErrorCodes::CATEGORY_STAGE, 0x1001,
                    "AutoStage: Coordinates saved successfully",
                    "File: " + filepath
                );
            }
        }
    }

    // Callback: Cycle parameter changed
    static void cycleParamChangedCallback(Fl_Widget* w, void* data) {
        TabAutoStage* tab = static_cast<TabAutoStage*>(data);
        tab->generateCoordinatesFromCycle();
    }

    // Callback: Coordinate spinner changed (shift through coordinates)
    static void coordinateShiftCallback(Fl_Widget* w, void* data) {
        TabAutoStage* tab = static_cast<TabAutoStage*>(data);
        
        try {
            int new_index = std::stoi(tab->coord_spinner[0]->value());
            if (new_index < 0) new_index = 0;
            if (new_index >= (int)tab->internal_coordinates.size()) {
                new_index = std::max(0, (int)tab->internal_coordinates.size() - 1);
            }
            tab->coord_view_index = new_index;
            tab->updateCoordinateDisplay();
        } catch (...) {
            // Ignore invalid input
        }
    }

    // Callback: Start AutoStage measurement
    static void startAutoStageCallback(Fl_Widget* w, void* data) {
        TabAutoStage* tab = static_cast<TabAutoStage*>(data);
        
        if (tab->btn_start->value()) {
            // Starting
            if (tab->internal_coordinates.empty()) {
                ErrorLogger::GetInstance().LogError(
                    ErrorCodes::CATEGORY_STAGE, 0x0005,
                    "AutoStage: No coordinates to measure",
                    "Load or generate coordinates first"
                );
                tab->btn_start->value(0);
                return;
            }
            
            tab->is_running = true;
            tab->start_time = std::chrono::steady_clock::now();
            FBConnector::get().enqueueTask([tab]() {
                tab->runAutoStageMeasurement();
            });
        } else {
            // Stopping (not implemented yet)
            tab->is_running = false;
        }
    }

    // Dummy backend measurement function
    void runAutoStageMeasurement() {
        int total = internal_coordinates.size();
        
        for (int i = 0; i < total && is_running; i++) {
            // Update UI with current coordinate
            FBConnector::get().enqueueUIUpdate([this, i]() {
                stat_iterator->value(std::to_string(i + 1).c_str());
                updateExcitationDisplay(i);
                
                float progress = (float)(i + 1) / (float)internal_coordinates.size() * 100.0f;
                progress_bar->value(progress);
            });
            
            // Simulate measurement time per coordinate (5 seconds dummy)
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        // Measurement complete
        FBConnector::get().enqueueUIUpdate([this]() {
            is_running = false;
            btn_start->value(0);
            progress_bar->value(100.0f);
            ErrorLogger::GetInstance().LogError(
                ErrorCodes::CATEGORY_STAGE, 0x1002,
                "AutoStage: Measurement completed",
                "Processed " + std::to_string(internal_coordinates.size()) + " coordinates"
            );
        });
    }

public:
    TabAutoStage(int X, int Y, int W, int H, const char* L = 0) 
        : Fl_Group(X, Y, W, H, L), is_running(false), coord_view_index(0) {
        
        int cx = X + W / 2;
        
        // -------------------------------------------------------------
        // TOP: Start / Save Toggles
        // -------------------------------------------------------------
        btn_start = new StageToggle(cx - 90, Y + 15, 70, 25, "start");
        btn_start->callback(startAutoStageCallback, this);
        
        StageToggle* btn_save = new StageToggle(cx + 20, Y + 15, 70, 25, "save");
        btn_save->callback(saveCoordinatesCallback, this);

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
            cycle_step[i] = iStep;
            iStep->callback(cycleParamChangedCallback, this);

            Fl_Int_Input* iMod = new Fl_Int_Input(cyc_x + 135, yy, 60, 25);
            iMod->value(defCyc[i]);
            cycle_mod[i] = iMod;
            iMod->callback(cycleParamChangedCallback, this);

            Fl_Float_Input* iRMin = new Fl_Float_Input(cyc_x + 210, yy, 50, 25);
            iRMin->value(defRangeMin[i]);
            cycle_range_min[i] = iRMin;
            iRMin->callback(cycleParamChangedCallback, this);

            Fl_Float_Input* iRMax = new Fl_Float_Input(cyc_x + 270, yy, 50, 25);
            iRMax->value(defRangeMax[i]);
            cycle_range_max[i] = iRMax;
            iRMax->callback(cycleParamChangedCallback, this);
        }
        grp_cycle->end();

        // -------------------------------------------------------------
        // PROGRESS BAR
        // -------------------------------------------------------------
        progress_bar = new Fl_Progress(cyc_x, cyc_y + cyc_h + 12, cyc_w, 10);
        progress_bar->selection_color(FL_BLUE);
        progress_bar->value(0);

        // -------------------------------------------------------------
        // MID SECTION: Load/Save/Measurement Type in one row
        // -------------------------------------------------------------
        int mid_y = cyc_y + cyc_h + 45;
        
        // Load / Save Toggle in one row
        StageToggle* btn_load = new StageToggle(cx - 180, mid_y, 65, 25, "load");
        btn_load->callback(loadCoordinatesCallback, this);
        
        new StageToggle(cx - 105, mid_y, 65, 25, "save");

        // Measurement Type in same row
        Fl_Box* lbl_meas = new Fl_Box(cx - 20, mid_y, 100, 20, "measurement type");
        lbl_meas->labelsize(12); lbl_meas->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        Fl_Choice* ch_meas = new Fl_Choice(cx - 20, mid_y + 18, 110, 25);
        ch_meas->add("spectrum");
        ch_meas->value(0);

        // Coordinates Section (moved down and to the left)
        int coord_y = mid_y + 65;
        Fl_Box* lbl_coord = new Fl_Box(cx - 180, coord_y, 100, 20, "coordinates [µm]");
        lbl_coord->labelsize(12);
        
        // Spinners for shifting through coordinates (index input)
        Fl_Box* lbl_coord_idx = new Fl_Box(cx - 180, coord_y + 20, 65, 18, "index");
        lbl_coord_idx->labelsize(10);
        coord_spinner[0] = new Fl_Int_Input(cx - 180, coord_y + 38, 65, 22);
        coord_spinner[0]->value("0");
        coord_spinner[0]->callback(coordinateShiftCallback, this);
        
        // 3x3 Table - displays 3 consecutive coordinates with 3 decimal places
        Fl_Group* coord_tbl = new Fl_Group(cx - 105, coord_y + 25, 140, 80);
        coord_tbl->box(FL_ENGRAVED_FRAME);
        coord_view_index = 0;
        for(int r=0; r<3; r++) {
            for(int c=0; c<3; c++) {
                Fl_Input* in = new Fl_Input(cx - 105 + c*45 + 2, coord_y + 25 + r*26 + 2, 41, 22);
                in->value("0.000");
                in->readonly(1);  // Read-only display
                coord_display[r][c] = in;
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
                excitation_display[r][c] = in; // Store for live updates
            }
        }
        exc_tbl->end();

        // -------------------------------------------------------------
        // BOTTOM STATS
        // -------------------------------------------------------------
        int stat_y = exc_y + 92;
        
        Fl_Box* lbl_iter = new Fl_Box(cx - 150, stat_y, 70, 20, "iterator");
        lbl_iter->labelsize(12); lbl_iter->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        stat_iterator = new Fl_Int_Input(cx - 150, stat_y + 18, 80, 24);
        stat_iterator->value("0"); 
        stat_iterator->readonly(1); 
        stat_iterator->textsize(13);

        Fl_Box* lbl_tot = new Fl_Box(cx - 40, stat_y, 70, 20, "total");
        lbl_tot->labelsize(12); lbl_tot->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        stat_total = new Fl_Int_Input(cx - 40, stat_y + 18, 80, 24);
        stat_total->value("0"); 
        stat_total->readonly(1); 
        stat_total->textsize(13);

        Fl_Box* lbl_time = new Fl_Box(cx + 70, stat_y, 80, 20, "time/min");
        lbl_time->labelsize(12); lbl_time->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        stat_time = new Fl_Int_Input(cx + 70, stat_y + 18, 80, 24);
        stat_time->value("0"); 
        stat_time->readonly(1); 
        stat_time->textsize(13);

        // -------------------------------------------------------------
        // COORDINATE GRAPH PLACEHOLDER
        // -------------------------------------------------------------
        int g_y = stat_y + 48;
        Fl_Box* lbl_graph = new Fl_Box(cyc_x, g_y, 110, 20, "Coordinate Graph");
        lbl_graph->labelsize(12); lbl_graph->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Group* grp_graph = new Fl_Group(cyc_x, g_y + 20, cyc_w, 160);
        grp_graph->box(FL_ENGRAVED_FRAME);
        
        int plot_x = cyc_x + 24;
        int plot_y = g_y + 34;
        int plot_w = cyc_w - 80;
        int plot_h = 136;
        
        // Instantiate the custom Fancy Coordinate Graph
        CoordinateGraph* coord_graph = new CoordinateGraph(plot_x, plot_y, plot_w, plot_h, "");
        
        // Dummy legend
        Fl_Box* grad = new Fl_Box(FL_FLAT_BOX, cyc_x + cyc_w - 32, g_y + 44, 12, 120, "");
        grad->color(FL_BLUE); // Gradient placeholder

        grp_graph->end();

        end();
    }
};
