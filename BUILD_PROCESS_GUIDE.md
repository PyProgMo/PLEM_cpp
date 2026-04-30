# Build Process with DLL Compatibility Checking

## Overview

The build system now includes automatic DLL architecture compatibility verification. This prevents the 0xc000007b error (32-bit/64-bit mismatch) before deployment.

## New Build Process

```
1. Pre-build Compatibility Check
   ↓
   [DLL_DIAGNOSTICS.sh runs automatically]
   - Verifies MinGW compiler is 64-bit
   - Checks FLTK installation exists
   - Validates all DLLs are 64-bit (not 32-bit)
   - Reports any mismatches
   ↓
2. If checks pass → Proceed to compilation
   If checks fail → Abort build with error message
   ↓
3. Compilation
   ↓
4. Smart DLL Deployment
   - Auto-detect FLTK path
   - Verify each DLL is 64-bit before copying
   - Report success/failure for each DLL
   ↓
5. Final Verification
   - Count deployed DLLs
   - Alert if any missing
   ↓
6. Optional: Run executable
```

## Scripts

### build_windows.sh (Enhanced)

**What it does now:**
1. ✅ Checks MinGW64 64-bit compiler exists
2. ✅ Runs `DLL_DIAGNOSTICS.sh` before build
3. ✅ Compiles the executable
4. ✅ Auto-detects FLTK installation path
5. ✅ Validates each DLL is 64-bit before copying
6. ✅ Reports deployment status
7. ✅ Alerts if missing DLLs

**Usage:**
```bash
./build_windows.sh
```

**Output example (success):**
```
Running pre-build compatibility check...
=================================================
DLL Architecture Compatibility Diagnostics
=================================================

1. Checking MinGW64 Installation
   Compiler check...
   ✓ 64-bit MinGW compiler found: /mingw64/bin/x86_64-w64-mingw32-g++

2. Checking FLTK Installation
   ✓ FLTK found at: /mingw64
   FLTK Version: 1.4.0

3. Checking 64-bit FLTK Runtime DLLs
   Scanning: /mingw64/bin/
   ✓ 64-bit: libfltk.dll
   ✓ 64-bit: libfltk_images.dll
   ✓ 64-bit: libpng16.dll
   ✓ 64-bit: libz.dll

4. Checking Thorlabs Power Meter DLL
   ✓ 64-bit: TLPM_64.dll

==================================================
Summary & Recommendations
==================================================

✓ All checks passed

Configuration is compatible:
• Compiler: 64-bit MinGW64
• FLTK: 64-bit installed
• DLLs: All 64-bit
• Thorlabs DLL: 64-bit

You can safely run: ./build_windows.sh
```

**Output example (failure - mixed architectures):**
```
3. Checking 64-bit FLTK Runtime DLLs
   Scanning: /mingw64/bin/
   ✓ 64-bit: libfltk.dll
   ✗ 32-bit: libfltk_images.dll (INCOMPATIBLE - should be 64-bit)
   ✗ 32-bit: libpng16.dll (INCOMPATIBLE - should be 64-bit)
   ✗ 32-bit: libz.dll (INCOMPATIBLE - should be 64-bit)

==================================================
Summary & Recommendations
==================================================

ERROR: DLL compatibility issues detected

Issues found:
1. Some DLLs are 32-bit when 64-bit is required
2. Some DLLs are missing

Solutions:
• Remove 32-bit FLTK: pacman -R mingw-w64-i686-fltk
• Install 64-bit FLTK: pacman -S mingw-w64-x86_64-fltk
• Verify installation: pacman -Q mingw-w64-x86_64-fltk
```

### DLL_DIAGNOSTICS.sh (Standalone)

**What it does:**
- Checks MinGW compiler version
- Finds FLTK installation
- Verifies each DLL is 64-bit (PE32+) or reports if 32-bit (PE32)
- Lists missing DLLs
- Provides remediation steps

**Usage (standalone):**
```bash
bash DLL_DIAGNOSTICS.sh
```

**Run this manually to:**
- Troubleshoot before building
- Verify your installation
- Check what version of FLTK is installed
- Identify 32-bit vs 64-bit issues

## Architecture Verification

Each DLL is checked using the `file` command:

```bash
file /mingw64/bin/libfltk.dll
```

**Output examples:**

✅ **Correct (64-bit)**:
```
PE32+ executable (x86-64, dynamically linked)
```

❌ **Wrong (32-bit)**:
```
PE32 executable (Intel 80386, dynamically linked)
```

## Common Issues & Solutions

### Issue 1: "DLL not found" in diagnostics

**Error message:**
```
✗ NOT FOUND: libfltk.dll
ERROR: FLTK not found
```

**Solution:**
```bash
pacman -S mingw-w64-x86_64-fltk
```

### Issue 2: "32-bit DLL" detected

**Error message:**
```
✗ 32-bit: libfltk.dll (INCOMPATIBLE - should be 64-bit)
```

**Solution:**
```bash
# Remove 32-bit
pacman -R mingw-w64-i686-fltk

# Install 64-bit
pacman -S mingw-w64-x86_64-fltk
```

### Issue 3: Build succeeds but .exe crashes with 0xc000007b

**Cause**: DLLs were copied but are 32-bit

**Solution**:
```bash
# Check deployed DLLs
file build/windows/*.dll

# If any show "PE32" (not PE32+), they are 32-bit
# Run diagnostics to find the issue
bash DLL_DIAGNOSTICS.sh

# Follow remediation steps
```

## Build System Comparison

### Before (Manual DLL copy)
```
❌ No architecture validation
❌ Hardcoded /mingw64/bin path
❌ Silent failures (DLL not found → application crash)
❌ Difficult to diagnose
❌ No pre-build checking
```

### After (Smart DLL deployment)
```
✅ Automatic architecture validation
✅ Dynamic FLTK path detection
✅ Pre-build compatibility check
✅ Clear error messages
✅ DLL verification before copy
✅ Architecture mismatch detection
✅ Detailed deployment report
```

## Deployment Checklist

Before running your application:

- [ ] Run `./build_windows.sh` (includes diagnostics)
- [ ] Check output for ✓ green checkmarks
- [ ] Verify `build/windows/` contains 5 DLLs (or 4 if libpng.dll instead of libpng16.dll)
- [ ] Verify `file build/windows/*.dll` shows "PE32+ executable" for all
- [ ] Test: `./build/windows/Template.exe` (from cmd, not MSYS2)
- [ ] If 0xc000007b error: Read [ERROR_0xC000007B_GUIDE.md](ERROR_0xC000007B_GUIDE.md)

## Troubleshooting Reference

| Error | Cause | Solution |
|-------|-------|----------|
| "MinGW 64-bit compiler not found" | Wrong terminal | Use MSYS2 MinGW 64-bit (not 32-bit) |
| "FLTK not found" | FLTK not installed | `pacman -S mingw-w64-x86_64-fltk` |
| "32-bit DLL detected" | Mixed installation | Remove i686, keep x86_64 |
| "Copy failed" | Permissions | Close exe and try again |
| 0xc000007b on .exe run | 32-bit DLL used | See ERROR_0xC000007B_GUIDE.md |

## Technical Details

### How Architecture Check Works

```bash
file build/windows/libfltk.dll | grep -q "PE32+"
```

- Returns 0 (success) = 64-bit (PE32+)
- Returns 1 (failure) = 32-bit (PE32) or other format

### How FLTK Path Detection Works

```bash
for path in /mingw64 /usr/local/opt/fltk /opt/fltk; do
    if [ -f "$path/bin/libfltk.dll" ]; then
        FLTK_BIN="$path/bin"
        break
    fi
done
```

Searches standard installation paths to find where FLTK is actually installed.

### Safe DLL Copy Function

```bash
copy_dll_safe() {
    # 1. Check if file exists
    if [ ! -f "$source" ]; then
        return 1
    fi
    
    # 2. Check if 64-bit
    if file "$source" | grep -q "PE32+"; then
        cp "$source" "$dest"
        return 0
    fi
    
    # 3. Report if wrong architecture
    return 1
}
```

Only copies if file exists AND is 64-bit. Reports if skipped.

---

## See Also

- [ERROR_0xC000007B_GUIDE.md](ERROR_0xC000007B_GUIDE.md) - Error 0xc000007b troubleshooting
- [DLL_REFERENCE.md](DLL_REFERENCE.md) - Complete DLL reference
- [64BIT_MIGRATION.md](64BIT_MIGRATION.md) - Architecture migration guide
