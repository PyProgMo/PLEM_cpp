# Debug Diagnostics

This folder contains standalone runtime diagnostics for Windows build issues.

## 0xc000007b checker

Run the dependency scanner against the built executable:

```bash
bash debug/diagnose_0xc000007b.sh
```

Optional target path:

```bash
bash debug/diagnose_0xc000007b.sh build/windows/PLEMv5.exe
```

The script focuses on the executable and the DLLs in `build/windows/` and `build/dlls/`, then reports only missing or 32-bit dependencies unless everything passes.

If a dependency exists only in `/mingw64/bin` or `/mingw32/bin`, the script reports it as `DEV-ONLY` because it will not be available when the app is started normally from Explorer.