#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Box.H>
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include "../include/metrics.h"

std::vector<DataItem> data_items;

std::vector<DataItem> process_folder(const std::string& folder) {
    std::vector<DataItem> result;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            result.push_back({
                entry.path().filename().string(),
                std::to_string(entry.file_size()) + " bytes",
                entry.path().extension().string(),
                "Ready"
            });
        }
    }
    return result;
}

void load_files(const std::string& folder) {
    data_items = process_folder(folder);
}

class TemplateWindow : public Fl_Window {
    Fl_Tabs* tabs;
    Fl_Group* page1;
    Fl_Group* page2;
    Fl_Input* folder_input;
    Fl_Button* load_btn;
    Fl_Button* process_btn;
    Fl_Table* table;
    Fl_Text_Display* text_display;
    Fl_Text_Buffer* text_buffer;
    std::string selected_folder;
public:
    TemplateWindow(int W, int H, const char* title = 0) : Fl_Window(W, H, title) {
        tabs = new Fl_Tabs(10, 10, W-20, H-20);
        
        // Page 1: File Operations
        page1 = new Fl_Group(20, 40, W-40, H-60, "File Operations");
        
        // Create separate label for better positioning
        Fl_Box* folder_label = new Fl_Box(50, 70, 60, 30, "Folder:");
        folder_label->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
        
        folder_input = new Fl_Input(120, 70, 300, 30);
        folder_input->value(std::filesystem::current_path().string().c_str()); // Auto-detect current directory
        load_btn = new Fl_Button(430, 70, 80, 30, "Load");
        process_btn = new Fl_Button(520, 70, 80, 30, "Process");
        
        // Text display for status/results
        text_buffer = new Fl_Text_Buffer();
        text_display = new Fl_Text_Display(30, 110, W-60, 200);
        text_display->buffer(text_buffer);
        text_buffer->text("FLTK Template Application\nSelect a folder and click Load to begin.");
        
        load_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->selected_folder = win->folder_input->value();
            load_files(win->selected_folder);
            win->update_display();
            win->update_table();
        }, this);
        
        process_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->process_data();
        }, this);
        
        page1->end();
        
        // Page 2: Data Table
        page2 = new Fl_Group(20, 40, W-40, H-60, "Data View");
        table = new Fl_Table(30, 70, W-60, H-100);
        table->rows(0);
        table->cols(4);
        table->col_header(1);
        table->row_header(1);
        table->col_width_all(150);
        page2->end();
        
        tabs->end();
        end();
    }
    
    void update_display() {
        std::string status = "Loaded " + std::to_string(data_items.size()) + " items from: " + selected_folder;
        text_buffer->text(status.c_str());
    }
    
    void update_table() {
        table->rows(data_items.size());
        redraw();
    }
    
    void process_data() {
        text_buffer->text("Processing data... (implement your custom logic here)");
        // Add your processing logic here
    }
};

int main(int argc, char** argv) {
    TemplateWindow win(800, 600, "FLTK Template Application");
    win.show(argc, argv);
    return Fl::run();
}
