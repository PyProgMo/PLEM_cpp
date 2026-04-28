#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include "frontend/FrontendConnection.h"
#include "../include/DebugConsole.h"

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
    
    // Initialize and start the Debug Console
    DebugConsole debugConsole;
    debugConsole.start();
    
    // Example: If we had a ThorlabsPM instance here, we would register it like this:
    // debugConsole.registerComponent("thorlabs", deviceInstance);

    FrontendConnection* window = new FrontendConnection(1200, 800, "PLEM Multi-Pane Frontend");
    window->show(argc, argv);
    
    int result = Fl::run();
    
    // Cleanup
    debugConsole.stop();
    return result;
}
