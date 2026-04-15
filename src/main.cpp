#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_draw.H>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

struct DeviceState {
    bool laser_initialized = false;
    bool spectrometer_initialized = false;
    bool ccd1_initialized = false;
    bool ccd2_initialized = false;
    bool cooling_active = false;

    bool all_initialized() const {
        return laser_initialized && spectrometer_initialized && ccd1_initialized && ccd2_initialized;
    }
};

class HeatmapWidget : public Fl_Widget {
    std::vector<std::vector<double>>* matrix_;
public:
    HeatmapWidget(int X, int Y, int W, int H, std::vector<std::vector<double>>* matrix)
        : Fl_Widget(X, Y, W, H), matrix_(matrix) {}

    void draw() override {
        fl_push_clip(x(), y(), w(), h());
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());

        if (!matrix_ || matrix_->empty() || (*matrix_)[0].empty()) {
            fl_color(FL_DARK3);
            fl_draw("No 2D data available", x() + 8, y() + 20);
            fl_pop_clip();
            return;
        }

        const int rows = static_cast<int>(matrix_->size());
        const int cols = static_cast<int>((*matrix_)[0].size());
        const int cell_w = std::max(1, w() / cols);
        const int cell_h = std::max(1, h() / rows);

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                const double value = std::clamp((*matrix_)[r][c], 0.0, 1.0);
                const uchar red = static_cast<uchar>(255.0 * value);
                const uchar blue = static_cast<uchar>(255.0 * (1.0 - value));
                fl_color(fl_rgb_color(red, 0, blue));
                fl_rectf(x() + c * cell_w, y() + r * cell_h, cell_w, cell_h);
            }
        }

        fl_color(FL_BLACK);
        fl_rect(x(), y(), w(), h());
        fl_pop_clip();
    }
};

class TemplateWindow : public Fl_Window {
    Fl_Tabs* tabs;
    Fl_Group* control_page;
    Fl_Group* data_page;
    Fl_Button* init_btn;
    Fl_Button* deinit_btn;
    Fl_Button* cooling_on_btn;
    Fl_Button* cooling_off_btn;
    Fl_Button* measure_btn;
    Fl_Button* save_btn;
    Fl_Text_Display* status_display;
    Fl_Text_Display* spectrum_display;
    Fl_Text_Buffer* text_buffer;
    Fl_Text_Buffer* spectrum_buffer;
    HeatmapWidget* heatmap;
    DeviceState devices_;
    std::vector<double> spectrum_1d_;
    std::vector<std::vector<double>> spectrum_2d_;
public:
    TemplateWindow(int W, int H, const char* title = 0) : Fl_Window(W, H, title) {
        tabs = new Fl_Tabs(10, 10, W-20, H-20);

        control_page = new Fl_Group(20, 40, W-40, H-60, "Device Control");
        init_btn = new Fl_Button(40, 70, 150, 30, "Init Devices");
        deinit_btn = new Fl_Button(210, 70, 150, 30, "Deinit Devices");
        cooling_on_btn = new Fl_Button(380, 70, 170, 30, "Activate Cooling");
        cooling_off_btn = new Fl_Button(570, 70, 190, 30, "Deactivate Cooling");
        measure_btn = new Fl_Button(40, 115, 250, 30, "Acquire 1D + 2D Measurement");
        save_btn = new Fl_Button(310, 115, 180, 30, "Save Spectra");

        text_buffer = new Fl_Text_Buffer();
        status_display = new Fl_Text_Display(30, 165, W-60, H-235);
        status_display->buffer(text_buffer);
        text_buffer->text("Ready. Initialize devices before measurement.");

        init_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->init_devices();
        }, this);

        deinit_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->deinit_devices();
        }, this);

        cooling_on_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->activate_cooling();
        }, this);

        cooling_off_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->deactivate_cooling();
        }, this);

        measure_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->acquire_measurement();
        }, this);

        save_btn->callback([](Fl_Widget*, void* v) {
            TemplateWindow* win = (TemplateWindow*)v;
            win->save_spectra();
        }, this);

        control_page->end();

        data_page = new Fl_Group(20, 40, W-40, H-60, "Measurement Data");
        spectrum_buffer = new Fl_Text_Buffer();
        spectrum_display = new Fl_Text_Display(30, 70, W / 2 - 35, H - 120);
        spectrum_display->buffer(spectrum_buffer);
        spectrum_buffer->text("1D spectrum will be shown here.");
        heatmap = new HeatmapWidget(W / 2 + 5, 70, W / 2 - 35, H - 120, &spectrum_2d_);
        data_page->end();

        tabs->end();
        end();
    }

    ~TemplateWindow() override {
        deinit_devices();
    }

    int handle(int event) override {
        if (event == FL_CLOSE) {
            deinit_devices();
        }
        return Fl_Window::handle(event);
    }

    void set_status(const std::string& status) {
        text_buffer->text(status.c_str());
    }

    void init_devices() {
        devices_.laser_initialized = true;
        devices_.spectrometer_initialized = true;
        devices_.ccd1_initialized = true;
        devices_.ccd2_initialized = true;
        set_status("Devices initialized: laser, spectrometer, CCD camera 1, CCD camera 2.");
    }

    void deinit_devices() {
        if (!devices_.all_initialized() && !devices_.cooling_active) {
            return;
        }
        devices_.cooling_active = false;
        devices_.laser_initialized = false;
        devices_.spectrometer_initialized = false;
        devices_.ccd1_initialized = false;
        devices_.ccd2_initialized = false;
        set_status("Devices deinitialized. Cooling disabled.");
    }

    void activate_cooling() {
        if (!devices_.all_initialized()) {
            set_status("Cannot activate cooling: initialize all devices first.");
            return;
        }
        devices_.cooling_active = true;
        set_status("Cooling activated.");
    }

    void deactivate_cooling() {
        devices_.cooling_active = false;
        set_status("Cooling deactivated.");
    }

    void acquire_measurement() {
        if (!devices_.all_initialized()) {
            set_status("Cannot acquire measurement: initialize all devices first.");
            return;
        }
        spectrum_1d_.clear();
        spectrum_2d_.assign(16, std::vector<double>(16, 0.0));

        for (int i = 0; i < 128; ++i) {
            const double t = static_cast<double>(i) / 127.0;
            const double signal = 0.5 + 0.5 * std::sin(14.0 * t) * std::exp(-1.8 * t);
            spectrum_1d_.push_back(std::clamp(signal, 0.0, 1.0));
        }

        for (size_t r = 0; r < spectrum_2d_.size(); ++r) {
            for (size_t c = 0; c < spectrum_2d_[r].size(); ++c) {
                const double x = static_cast<double>(c) / (spectrum_2d_[r].size() - 1);
                const double y = static_cast<double>(r) / (spectrum_2d_.size() - 1);
                const double peak = std::exp(-16.0 * ((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5)));
                spectrum_2d_[r][c] = std::clamp(0.15 + 0.85 * peak, 0.0, 1.0);
            }
        }

        std::ostringstream stream;
        stream << "Index\tIntensity\n";
        for (size_t i = 0; i < spectrum_1d_.size(); ++i) {
            stream << i << "\t" << std::fixed << std::setprecision(6) << spectrum_1d_[i] << "\n";
        }
        spectrum_buffer->text(stream.str().c_str());
        heatmap->redraw();

        set_status(devices_.cooling_active
            ? "Measurement acquired with cooling active."
            : "Measurement acquired (cooling inactive).");
    }

    void save_spectra() {
        if (spectrum_1d_.empty()) {
            set_status("No spectrum to save. Acquire measurement first.");
            return;
        }

        const char* path = fl_file_chooser("Save Spectrum CSV", "*.csv", "spectrum.csv");
        if (!path) {
            set_status("Save cancelled.");
            return;
        }

        std::ofstream out(path);
        if (!out.is_open()) {
            set_status("Failed to save spectrum file.");
            return;
        }

        out << "index,intensity\n";
        for (size_t i = 0; i < spectrum_1d_.size(); ++i) {
            out << i << "," << std::fixed << std::setprecision(6) << spectrum_1d_[i] << "\n";
        }
        out.close();
        set_status(std::string("Spectrum saved to: ") + path);
    }
};

int main(int argc, char** argv) {
    TemplateWindow win(900, 650, "HSI Measurement Control");
    win.show(argc, argv);
    return Fl::run();
}
