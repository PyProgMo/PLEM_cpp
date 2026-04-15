#include <FL/Fl.H>
#include "frontend/FrontendConnection.h"

int main(int argc, char** argv) {
    Fl::scheme("gtk+"); // Make the GUI look more modern
    FrontendConnection* window = new FrontendConnection(1200, 800, "PLEM Multi-Pane Frontend");
    window->color(FL_BACKGROUND_COLOR); // Explicitly set background color to prevent redraw artifacts
    window->show(argc, argv);
    return Fl::run();
}
