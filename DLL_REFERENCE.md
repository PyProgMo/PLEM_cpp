# DLL Reference Guide - 64-Bit Architecture Only

**Architecture Standard**: Exclusive 64-bit (x86_64)  
**Build System**: MSYS2 MinGW64  
**Status**: All DLLs automatically deployed during build

Quick reference for all DLL files used in the hsimeasurement project.

## Project Structure

```
standalone_devices/
  thorlabs_powermeter/
    dll/                    ← DLL files and headers
    Include/                ← Header files for compilation
    tl100d_reader/          ← Thorlabs reader implementation

build/
  windows/                  ← Output directory for executable and runtime DLLs
```

## DLL Summary

### Custom/Project DLLs

#### TLPM_64.dll
| Property | Value |
|----------|-------|
| **Location** | `standalone_devices/thorlabs_powermeter/dll/TLPM_64.dll` |
| **Output Location** | `build/windows/TLPM_64.dll` |
| **Architecture** | 64-bit (x86_64) |
| **Bit Version** | 64-bit required |
| **Purpose** | Thorlabs Power Meter PM100D driver |
| **Included in Repo** |  Yes |
| **Deployment** | Automatically copied by build script |

### Runtime DLLs Required (Now Auto-Copied)

These DLLs are from MSYS2 MinGW64 and are automatically copied by the updated build script.

#### libfltk.dll
| Property | Value |
|----------|-------|
| **Source** | MSYS2 MinGW64 (`/mingw64/bin/`) |
| **Architecture** | 64-bit (x86_64) |
| **Bit Version** | 64-bit required |
| **Purpose** | FLTK GUI framework (core library) |
| **Included in Repo** |  No (external dependency) |
| **Deployment** | Automatically copied to `build/windows/` by build script |
| **Status** |  Auto-copied - Available after build |

#### libfltk_images.dll
| Property | Value |
|----------|-------|
| **Source** | MSYS2 MinGW64 (`/mingw64/bin/`) |
| **Architecture** | 64-bit (x86_64) |
| **Bit Version** | 64-bit required |
| **Purpose** | FLTK image handling (PNG, JPEG, etc.) |
| **Included in Repo** |  No (external dependency) |
| **Deployment** | Automatically copied to `build/windows/` by build script |
| **Status** |  Auto-copied - Available after build |

#### libpng16.dll (or libpng.dll)
| Property | Value |
|----------|-------|
| **Source** | MSYS2 MinGW64 (`/mingw64/bin/`) |
| **Architecture** | 64-bit (x86_64) |
| **Bit Version** | 64-bit required |
| **Purpose** | PNG image format support |
| **Included in Repo** |  No (external dependency) |
| **Deployment** | Automatically copied to `build/windows/` by build script |
| **Notes** | Version depends on MinGW64 installation (libpng16 or libpng) |
| **Status** |  Auto-copied - Available after build |

#### libz.dll (zlib)
| Property | Value |
|----------|-------|
| **Source** | MSYS2 MinGW64 (`/mingw64/bin/`) |
| **Architecture** | 64-bit (x86_64) |
| **Bit Version** | 64-bit required |
| **Purpose** | Compression library (dependency of libpng) |
| **Included in Repo** |  No (external dependency) |
| **Deployment** | Automatically copied to `build/windows/` by build script |
| **Status** |  Auto-copied - Available after build |

## Build Process (Automated Deployment)

The build script `build_windows.sh` now automatically handles all DLL deployment:

1. **Compiles** executable with `x86_64-w64-mingw32-g++` (64-bit)
2. **Copies** `TLPM_64.dll` from project source
3. **Copies** all 64-bit FLTK runtime DLLs from `/mingw64/bin/`
4. **Verifies** all DLLs are in `build/windows/`

**Result**: Application runs without missing DLL errors.

## Deployment Instructions

### Automatic Deployment (Recommended)

Simply run the build script:
```bash
./build_windows.sh
```

The script handles:
-  Compilation with 64-bit compiler
-  Copying Thorlabs Power Meter driver (TLPM_64.dll)
-  Copying all 64-bit FLTK runtime DLLs from `/mingw64/bin/`
-  Creating complete deployment package in `build/windows/`

### Manual Verification

After build, verify all DLLs are present:
```bash
ls -la build/windows/*.dll
```

Expected 5 files:
```
build/windows/libfltk.dll
build/windows/libfltk_images.dll
build/windows/libpng16.dll
build/windows/libz.dll
build/windows/TLPM_64.dll
```

### Deployment to Target Machine

1. Copy entire `build/windows/` folder to target machine
2. Run `PLEMv5.exe` - should start GUI without DLL errors
3. No additional installation required

---

## 64-Bit Architecture - Exclusive Standard

**All components use 64-bit (x86_64) architecture ONLY**

| Component | Architecture | Standard |
|-----------|--------------|----------|
| **Compiler** | 64-bit (x86_64) | `x86_64-w64-mingw32-g++` |
| **Executable** | 64-bit (x86_64) | PE32+ format |
| **All DLLs** | 64-bit (x86_64) | From `/mingw64/bin/` only |
| **Target Machines** | 64-bit (x86_64) | Windows 7 SP1 or later |

### Why Exclusively 64-Bit?

 Target machines are 64-bit capable  
 Better performance on modern CPUs  
 Simpler deployment (one binary set)  
 Reduced support complexity  
 No 32-bit confusion or mismatches  

### Never Use 32-Bit

 DO NOT use `/mingw32/bin/` DLLs  
 DO NOT use `i686-w64-mingw32-g++` compiler  
 DO NOT mix architectures  

**Result of mixing**: Crashes, "Entry Point Not Found", runtime failures
