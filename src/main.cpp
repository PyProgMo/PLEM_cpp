#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include "frontend/FrontendConnection.h"
#include "../include/DebugConsole.h"
#include "../standalone_devices/thorlabs_powermeter/tl100d_reader/src/ThorlabsPM.h"
#include "backend/AndorBackend.h"
#include "fbconnector/FBConnector.h"
#include "../include/ErrorLogger.h"
#include <string>

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
    
    ThorlabsPM* powermeter = new ThorlabsPM(PMMode::CONSOLE);
    debugConsole.registerComponent("thorlabs", powermeter);

    AndorBackend* andorBackend = new AndorBackend();

    // Start the FBConnector engine and register components for shutdown
    FBConnector::get().setDebugConsole(&debugConsole);
    FBConnector::get().start();

    FrontendConnection* window = new FrontendConnection(1200, 800, "PLEM Multi-Pane Frontend", &debugConsole, powermeter, andorBackend);
    
    // Register the main window with FBConnector for proper shutdown closure
    FBConnector::get().setMainWindow(window);
    
    window->show(argc, argv);
    
    int result = Fl::run();
    
    // Cleanup (may not be reached if shutdown is initiated via UI, but kept for safety)
    FBConnector::get().stop();
    debugConsole.stop();
    delete powermeter;
    delete andorBackend;
    return result;
}
