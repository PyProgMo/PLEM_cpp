# Complete Program Structure

## 1) High-Level Runtime Architecture

The executable is started from `src/main.cpp` and wires together:

1. **UI layer (FLTK)** via `FrontendConnection`
2. **Device backends/wrappers** (e.g., `AndorBackend`, `ThorlabsPM`)
3. **Async connector** (`FBConnector`) for non-blocking device calls
4. **Debug routing** (`DebugConsole` + `IDebuggable`)

### Startup flow
- `main.cpp` creates `DebugConsole`
- creates and registers hardware components (`thorlabs`, `andor`)
- starts `FBConnector`
- creates `FrontendConnection` (multi-pane tab UI)
- enters `Fl::run()`

### Shutdown flow
- UI-triggered or normal exit
- `FBConnector::stop()`
- `DebugConsole::stop()`
- backend device objects deleted

---

## 2) Source Tree and Responsibilities

### Core folders
- `src/main.cpp` — process entry point and top-level wiring
- `src/frontend/` — FLTK window and tabs
- `src/frontend/tabs/` — feature-specific UI panels
- `src/backend/` — backend wrappers and device-facing logic
- `src/fbconnector/` — async queue (backend worker + UI update queue)
- `include/` — shared interfaces/utilities (`IDebuggable`, logging, etc.)

### Key components
- `FrontendConnection.h`
  - Builds left control tabs and right display/control tabs
  - Hosts tabs like `TabInit`, `TabMeasurement`, `TabSpectrum`, `TabImage`, `TabPLEM`
- `FBConnector.{h,cpp}`
  - `enqueueTask(...)` for backend work on worker thread
  - `enqueueUIUpdate(...)` for main-thread FLTK updates
  - 60 FPS UI queue processing timer
- `AndorBackend.{h,cpp}`
  - camera init/deinit/status/cooling
  - spectrograph-style operations (slit/grating/filter/shutter/wavelength)
- `TabConfiguration.h`
  - spectrograph controls wired to `AndorBackend` via `FBConnector`
- `TabInit.h`
  - init/deinit toggles for powermeter and cameras
- `TabMeasurement.h`
  - measurement control buttons (`measure`, `Abort`, etc.)
- `TabSpectrum.h` / `TabImage.h`
  - display panels for 1D spectrum and image-like data placeholders
- `TabPLEM.h`
  - PLE scan UI and 2D color graph placeholder

---

## 3) Current Control and Display Surface

### Init/control
- Device init and status LEDs in `TabInit`
- Spectrograph and lightsource controls in `TabConfiguration`

### Measurement controls
- `TabMeasurement`: `measure`, `Abort`, `continuous-save`, `align-mode`
- `TabAcquisitionSetup`: state indicators and progress area

### Display
- `TabSpectrum`: 1D spectrum area
- `TabImage`: 2D image area
- `TabPLEM`: 2D PLEM color graph mock

---

## 4) Integration Philosophy

This project should remain an **integration shell**:
- external device APIs/drivers remain in dedicated wrappers
- UI never blocks on hardware calls
- all heavy I/O occurs through backend wrappers and `FBConnector`
- UI updates are marshaled back to main thread only

This is the required basis for high-throughput, low-latency measurement modes.
