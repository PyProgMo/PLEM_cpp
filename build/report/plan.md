## Plan: FLTK PLEM Frontend Architecture

This document tracks the iterative implementation of the PLEM frontend in C++ using FLTK.

**Tasks**
- [ ] Step 1: Restructure the Tabbed Interface
      - Remove old Device Control / Measurement Data tabs.
      - Add Measure, Align, Image, Calibrate, and Auto-Stage tabs.
- [ ] Step 2: Implement Measurement Controls (Measure Tab)
      - Add Fl_Value_Input for Wavelength Range (Start, Stop, Step).
      - Add Fl_Value_Input for Camera Settings (Integration time).
      - Add Fl_Value_Input for Laser Settings (Power level).
- [ ] Step 3: Implement Stage Controls (Align & Auto-Stage Tabs)
      - Add inputs for X, Y, and Z axes to control the PI nanostage.
- [ ] Step 4: Upgrade Data Visualization
      - Build a custom Fl_Widget for the 1D Spectrum Graph (axes, ticks, line drawing).
      - Upgrade HeatmapWidget for the 2D Map Display (aspect ratios, interactions).
- [ ] Step 5: State Machine & Backend Integration Placeholders
      - Add Fl_Light_Button for "Laser Ready" / "Laser Emit".
      - Add Fl_Light_Button for "Align Mode Active".
      - Add Start/Stop measurement buttons and a global status bar.