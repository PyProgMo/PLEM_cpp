# 64-Bit Architecture Migration Guide

**Date**: 2026-04-30  
**Status**: Implemented  
**Target Architecture**: 64-bit (x86_64) exclusively  
**Minimum Target Machine**: Old but not too old 64-bit systems

## Migration Overview

This document outlines the migration to exclusive 64-bit DLL support for the hsimeasurement project. All DLLs, executables, and dependencies are now standardized on 64-bit (x86_64) architecture.

## Why 64-Bit Only?

-  Target machines are old but 64-bit capable
-  64-bit provides better performance and memory addressing
-  Simplifies deployment (no need to maintain 32-bit variants)
-  Reduces build complexity and confusion
-  Modern Windows supports 64-bit natively

## Architecture Specifications

### Build Configuration
| Component | Architecture | Requirement |
|-----------|--------------|-------------|
| **Compiler** | x86_64-w64-mingw32 | 64-bit MinGW64 |
| **Executable** | x86_64 | 64-bit .exe |
| **All DLLs** | x86_64 | 64-bit .dll |
| **Target Machines** | x86_64 | 64-bit Windows 7 or later |

### Compilation Flag
```bash
COMPILER="x86_64-w64-mingw32-g++"  # 64-bit compiler ONLY
```

## 64-Bit DLL Mapping

All runtime DLLs are sourced from MSYS2 MinGW64 64-bit installation:

### Source Paths (64-bit)
```
/mingw64/bin/                    ← 64-bit DLLs (x86_64)
```

### Complete 64-Bit DLL List

| DLL Name | Source Path | Bit | Purpose |
|----------|-------------|-----|---------|
| **TLPM_64.dll** | `standalone_devices/thorlabs_powermeter/dll/` | 64-bit | Thorlabs Power Meter |
| **libfltk.dll** | `/mingw64/bin/` | 64-bit | FLTK Core GUI |
| **libfltk_images.dll** | `/mingw64/bin/` | 64-bit | FLTK Image Support |
| **libpng16.dll** | `/mingw64/bin/` | 64-bit | PNG Image Format |
| **libz.dll** | `/mingw64/bin/` | 64-bit | Compression Library |

### Deployment Location (64-bit)
```
build/windows/
├── Template.exe           (64-bit executable)
├── TLPM_64.dll           (64-bit)
├── libfltk.dll           (64-bit)
├── libfltk_images.dll    (64-bit)
├── libpng16.dll          (64-bit)
└── libz.dll              (64-bit)
```

##  DEPRECATED: 32-Bit Support

**DO NOT USE 32-BIT DLLs** from `/mingw32/bin/` directory.

### Why 32-Bit is Removed
- Not compatible with 64-bit executable (compilation error or runtime crash)
- Maintenance burden for unsupported architecture
- No performance benefit on 64-bit machines

### Forbidden Paths
```
/mingw32/bin/             ←  NEVER use 32-bit DLLs from here
```

## Migration Checklist

- [x] Compiler: Use `x86_64-w64-mingw32-g++` (64-bit only)
- [x] Build Script: Updated `build_windows.sh` to copy 64-bit FLTK DLLs
- [x] DLL Documentation: Created comprehensive DLL_REFERENCE.md
- [x] Runtime Documentation: Updated RUNTIME_DEPENDENCIES.md
- [x] Verification: All DLLs from `/mingw64/bin/` (64-bit)
- [x] Testing: Application runs on target 64-bit machines
- [x] Archive: 32-bit support removed from documentation

## Implementation Changes

### 1. Build Script (`build_windows.sh`)

**Change**: Added automatic copying of 64-bit FLTK DLLs

```bash
# Copy 64-bit FLTK runtime libraries
echo "Copying 64-bit FLTK runtime DLLs..."
cp /mingw64/bin/libfltk.dll build/windows/ || true
cp /mingw64/bin/libfltk_images.dll build/windows/ || true
cp /mingw64/bin/libpng16.dll build/windows/ || true
cp /mingw64/bin/libz.dll build/windows/ || true
```

### 2. DLL Reference Documentation

**Change**: Explicitly labeled all DLLs as 64-bit, removed 32-bit options

### 3. Runtime Dependencies

**Change**: Clarified that only 64-bit DLLs from `/mingw64/bin/` are supported and are now automatically copied

## Verification Steps

### Before Building
1. Confirm MinGW64 64-bit is installed:
   ```bash
   x86_64-w64-mingw32-g++ --version
   ```
2. Verify 64-bit FLTK is installed:
   ```bash
   ls /mingw64/bin/libfltk.dll
   ```

### After Building
1. Check executable is 64-bit:
   ```bash
   file build/windows/Template.exe
   # Output should show: PE32+ executable (x86-64)
   ```

2. Verify all DLLs are 64-bit:
   ```bash
   ls -la build/windows/*.dll
   file build/windows/*.dll
   # All should show: PE32+ executable (x86-64)
   ```

3. Verify DLL names match expectations:
   ```bash
   ls build/windows/ | grep -E "\.dll$"
   # Expected output:
   # TLPM_64.dll
   # libfltk.dll
   # libfltk_images.dll
   # libpng16.dll
   # libz.dll
   ```

### Testing on Target Machine
1. Copy `build/windows/` folder to target machine
2. Run `Template.exe` - should start GUI without errors
3. No "DLL not found" errors should appear

## Troubleshooting

### Issue: Still Getting "DLL Not Found" Errors
- [ ] Check all DLLs in `build/windows/` are present
- [ ] Verify they are 64-bit (use `file` command)
- [ ] Ensure executable is also 64-bit

### Issue: "The application failed to initialize properly (0xc0000135)"
- [ ] This error indicates 32-bit DLLs used with 64-bit executable
- [ ] Verify all DLLs are from `/mingw64/bin/` NOT `/mingw32/bin/`
- [ ] Rebuild and recopy DLLs

### Issue: "Entry Point Not Found"
- [ ] DLL architecture mismatch
- [ ] Ensure all DLLs are 64-bit
- [ ] Check build script copied correct DLLs

## Performance Considerations

64-bit provides:
-  Better memory addressing for large files
-  Native support on modern Windows
-  Potential performance improvement on 64-bit CPUs
-  Simpler deployment (one binary, one set of DLLs)

## Future Maintenance

### When Updating FLTK
1. Ensure you install 64-bit version: `pacman -S mingw-w64-x86_64-fltk`
2. Do NOT install 32-bit version: `pacman -S mingw-w64-i686-fltk`
3. Rebuild and recopy DLLs using updated `build_windows.sh`

### Adding New Dependencies
1. Install 64-bit version in MSYS2 MinGW64
2. Add to `/mingw64/lib` linker path (if needed)
3. Add 64-bit DLL copy to `build_windows.sh`
4. Document in DLL_REFERENCE.md and this file

---

**Migration Completed**: All code paths now exclusively use 64-bit (x86_64) architecture.
