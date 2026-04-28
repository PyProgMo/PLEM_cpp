#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include "DebugConsole.h"
#include <string>

class TabDebugger : public Fl_Group {
public:
    TabDebugger(int X, int Y, int W, int H, const char* L, DebugConsole* dbgConsole)
        : Fl_Group(X, Y, W, H, L), m_console(dbgConsole) 
    {
        // 1. Text display for history/output
        m_txtDisplay = new Fl_Text_Display(X + 10, Y + 10, W - 20, H - 60);
        m_txtBuffer = new Fl_Text_Buffer();
        m_txtDisplay->buffer(m_txtBuffer);
        m_txtDisplay->textfont(FL_COURIER);
        m_txtDisplay->textsize(12);
        
        m_txtBuffer->append("Interactive Debugging Console.\nType 'help' to see registered components.\n\n");

        // 2. Command input field
        m_input = new Fl_Input(X + 10, Y + H - 40, W - 100, 30, "");
        m_input->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

        // 3. Send button
        m_btnSend = new Fl_Button(X + W - 80, Y + H - 40, 70, 30, "Send");
        
        // Callbacks
        m_btnSend->callback(on_send_cb, this);
        m_input->callback(on_send_cb, this); // Allow Enter key

        end();
    }

    ~TabDebugger() {
        delete m_txtBuffer; // Fl_Text_Display doesn't govern buffer lifecycle natively usually
    }

private:
    DebugConsole* m_console;
    Fl_Text_Display* m_txtDisplay;
    Fl_Text_Buffer*  m_txtBuffer;
    Fl_Input*        m_input;
    Fl_Button*       m_btnSend;

    static void on_send_cb(Fl_Widget* w, void* userdata) {
        TabDebugger* tab = static_cast<TabDebugger*>(userdata);
        tab->processInput();
    }

    void processInput() {
        if (!m_console) return;

        std::string line = m_input->value();
        if (line.empty()) return;

        // Echo the command to the display
        m_txtBuffer->append(("> " + line + "\n").c_str());

        // Get output from backend
        std::string out = m_console->executeCommand(line);
        if (!out.empty()) {
            m_txtBuffer->append(out.c_str());
            // Make sure the last line is a newline to space outputs
            if (out.back() != '\n') {
                m_txtBuffer->append("\n");
            }
        }

        // Clear input for next command
        m_input->value("");
        
        // Scroll to the bottom
        int lines = m_txtBuffer->count_lines(0, m_txtBuffer->length());
        m_txtDisplay->scroll(lines, 0);
    }
};