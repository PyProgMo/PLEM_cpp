#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include "frontend/FrontendConnection.h"

int main(int argc, char** argv) {
    Fl::scheme("gtk+"); // Make the GUI look more modern
    
    // Load and set the application icon
    Fl_RGB_Image* icon = new Fl_PNG_Image("icon1.png"); // if run from same directory
    if (icon->fail()) {
        delete icon;
        icon = new Fl_PNG_Image("build/graphics/icon1.png"); // if run from project root
    }
    
    if (!icon->fail()) {
        Fl_Window::default_icon(icon);
    } else {
        delete icon; // Fallback: no custom image will be loaded
    }
    
    FrontendConnection* window = new FrontendConnection(1200, 800, "PLEM Multi-Pane Frontend");
    window->show(argc, argv);
    return Fl::run();
}
