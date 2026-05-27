# Init/Measure/Display Flow and Next Steps

## 1) End-to-end flow target

### Step 1 — Init
- User initializes required devices (spectrometer, selected camera, laser, powermeter)
- UI confirms readiness per device (LED/state)

### Step 2 — Measure
- User starts measurement (single or scan sequence)
- Backend coordinates trigger/config/acquisition order
- User can abort safely

### Step 3 — Display
- 1D spectral result shown in spectrum panel
- 2D image/heatmap shown in image/PLE panel
- 3D-type data represented by slice/projection/stack navigation strategy

### Step 4 — Save/Export
- Save current spectrum and measurement products with metadata

---

## 2) Current implementation status (from codebase)

Already present:
- multi-tab control/display UI skeleton
- async backend task execution (`FBConnector`)
- initial Andor backend control methods
- init/deinit toggles and status LEDs

Still to build (core next implementation phase):
- complete measurement pipeline wiring from `measure` button to actual hardware acquisition
- data model unification for 1D/2D/3D outputs
- concrete rendering components replacing placeholders in `TabSpectrum` and `TabImage`
- robust synchronization across multiple devices during PLM/PLEM scans
- richer abort/recovery semantics for partial acquisition states

---

## 3) Next implementation roadmap

1. **Define common measurement data contracts**
   - standardized structs for spectrum/frame/scan metadata
2. **Implement acquisition orchestrator**
   - central sequence manager for multi-device timing
3. **Connect measurement buttons to orchestrator**
   - `measure`, `Abort`, and progress updates
4. **Implement display adapters**
   - convert raw device outputs into 1D/2D/3D display models
5. **Integrate remaining wrappers**
   - spectrometer, laser, additional cameras/devices
6. **Harden reliability path**
   - timeout handling, retries, clean shutdown on errors
7. **Benchmark fast mode**
   - measure cycle time, queue latency, and UI responsiveness

This roadmap should be executed while preserving the architecture rule: **wrappers do device-specific work, this project orchestrates and visualizes at high speed.**
