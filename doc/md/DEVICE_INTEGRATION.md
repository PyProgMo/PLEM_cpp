# Device Integration Guide (C++/DLL Wrappers)

## Objective
Integrate new hardware quickly and safely for fast measurement modes (PLM/PLEM and related workflows), while preserving UI responsiveness.

Supported target examples:
- spectrometer
- CCD/CMOS cameras
- laser/light source
- powermeter
- additional vendor devices

---

## 1) Where new device code should live

Create/extend a dedicated wrapper in a backend-oriented location (current pattern: device wrappers referenced from `src/backend/*` and `standalone_devices/*`).

### Wrapper responsibilities
- load/call vendor API (DLL/SO/dylib)
- expose clean C++ methods needed by this app
- perform device-specific validation and error conversion
- avoid direct FLTK dependencies

### App/backend responsibilities
- orchestrate wrapper calls
- map UI intent to wrapper operations
- push errors to `ErrorLogger`

---

## 2) Integrating an existing C++ "device talk" program

If you already have a C++ codebase that talks to the spectrometer (or another device):

1. Extract a stable API surface (init/configure/measure/read/shutdown).
2. Wrap that API in a class with methods aligned to this app’s actions.
3. Ensure ABI/build architecture consistency with this project (64-bit alignment).
4. Instantiate and own the wrapper in `main.cpp` (or in an owning backend object).
5. Inject the wrapper/backend into the tabs that need control.
6. Execute calls through `FBConnector::enqueueTask(...)`.
7. Update LEDs/fields/plots through `enqueueUIUpdate(...)` only.

---

## 3) Minimal interface shape for each new device

Use a consistent operation set per device type:
- `init()` / `deinit()`
- `isReady()` or `getStatus()`
- `configure(...)` (exposure, gain, wavelength, power, trigger, etc.)
- `measure()` or `acquireFrame()` / `acquireSpectrum()`
- `readData()` with timestamps and metadata
- optional `abort()`

For debugging and manual control, expose commands through `IDebuggable` when useful.

---

## 4) Wiring device functions to UI buttons (Init / Measure / Display)

### A) Init
- Add toggle/button in `TabInit` (or dedicated tab)
- callback schedules backend init/deinit on `FBConnector`
- UI LED/button state updated from UI queue

### B) Measure
- Bind `TabMeasurement` “measure” and “Abort” to backend measurement lifecycle
- run measurement loop asynchronously
- keep state indicators updated (`TabAcquisitionSetup`)

### C) Display spectra / image data
- Route 1D arrays to spectrum view (`TabSpectrum`)
- Route 2D matrices to image/heatmap view (`TabImage` / `TabPLEM` style panel)
- For 3D-like datasets (x, y, intensity or wavelength stack), define a backend data adapter that can produce:
  - 1D slices for spectrum
  - 2D projections/frames for image display
  - optional export-ready volume representation

---

## 5) Performance rules for FAST measurements

1. Never call hardware APIs directly on FLTK UI thread.
2. Preallocate reusable buffers for repeated acquisition.
3. Minimize copies between wrapper/backend/display layers.
4. Keep conversion/format transforms incremental and predictable.
5. Use clear state transitions: INIT → READY → ACQUIRING → COMPLETE/ERROR.
6. Emit concise structured errors to `ErrorLogger` with device category/code.

---

## 6) Practical checklist for adding one new device

- [ ] Define wrapper API (init/config/acquire/read/shutdown)
- [ ] Ensure architecture-compatible binary linkage (64-bit)
- [ ] Add backend orchestrator methods
- [ ] Inject backend into needed tabs
- [ ] Add UI controls (init + measure + status)
- [ ] Add async execution via `FBConnector`
- [ ] Add data mapping into spectrum/image/PLE displays
- [ ] Add error handling/logging and debug command hooks
- [ ] Validate startup/shutdown and failure recovery
