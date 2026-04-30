# Windows Error 0xc000007b - Troubleshooting Guide

**Error Code**: 0xc000007b (STATUS_INVALID_IMAGE_FORMAT)  
**Common Cause**: 32-bit and 64-bit DLL mismatch or incompatible library versions

## Quick Diagnosis

If you see error 0xc000007b when running `Template.exe`:

```
The application was unable to start correctly (0xc000007b)
```

### Root Causes

1. **32-bit DLL with 64-bit Executable** (Most Common)
   - Application is 64-bit but using 32-bit FLTK DLLs
   - Result: Immediate crash with 0xc000007b

2. **Corrupted or Incompatible DLLs**
   - Downloaded DLL is wrong version
   - DLL is from different FLTK installation
   - Dependency mismatch (libpng/zlib versions)

3. **Missing DLL Dependencies**
   - DLL chain is incomplete
   - Supporting library not in build/windows/ directory

## Solution Steps

### Step 1: Run Diagnostics (BEFORE rebuilding)

```bash
bash debug/diagnose_0xc000007b.sh
```

This script checks:
- ✓ Compiler architecture (must be 64-bit)
- ✓ FLTK installation (must be 64-bit)
- ✓ Each DLL bitness (must all be 64-bit)
- ✓ DLL availability

If you only need the build-time compatibility gate, you can still use:

```bash
bash DLL_DIAGNOSTICS.sh
```

### Step 2: Check Your FLTK Installation

```bash
# List installed FLTK versions
pacman -Q | grep fltk
```

You should see:
```
mingw-w64-x86_64-fltk <version>
```

**NOT**:
```
mingw-w64-i686-fltk <version>   ← This is 32-bit, WRONG!
```

### Step 3: Remove 32-Bit FLTK (If Present)

If you see `mingw-w64-i686-fltk` installed:

```bash
# Remove 32-bit FLTK
pacman -R mingw-w64-i686-fltk

# Verify it's gone
pacman -Q | grep fltk
```

Only `mingw-w64-x86_64-fltk` should remain.

### Step 4: Ensure 64-Bit FLTK is Installed

```bash
# Install or update 64-bit FLTK
pacman -S mingw-w64-x86_64-fltk

# Verify installation
pacman -Q mingw-w64-x86_64-fltk
```

### Step 5: Clean and Rebuild

```bash
# Delete old DLLs to force fresh copy
rm build/windows/*.dll

# Rebuild with new DLLs
./build_windows.sh
```

### Step 6: Verify DLL Compatibility

Check each DLL is 64-bit:

```bash
cd build/windows/
file *.dll
```

**Expected output** (all should show PE32+):
```
libfltk.dll: PE32+ executable (x86-64)...
libfltk_images.dll: PE32+ executable (x86-64)...
libpng16.dll: PE32+ executable (x86-64)...
libz.dll: PE32+ executable (x86-64)...
TLPM_64.dll: PE32+ executable (x86-64)...
```

**If you see PE32 (not PE32+)**: That's 32-bit - WRONG architecture!

### Step 7: Test the Executable

```bash
# Check executable is 64-bit
file Template.exe
# Should show: PE32+ executable (x86-64)

# Try running it
./Template.exe
```

## Complete Fix Checklist

Use this if all else fails:

```bash
# 1. Clean everything
rm -rf build/windows/Template.exe
rm build/windows/*.dll

# 2. Check compiler
x86_64-w64-mingw32-g++ --version
# Must show x86_64, NOT i686

# 3. Remove any 32-bit MinGW/FLTK
pacman -R mingw-w64-i686-gcc mingw-w64-i686-fltk 2>/dev/null || true

# 4. Install fresh 64-bit FLTK
pacman -S mingw-w64-x86_64-fltk

# 5. Verify FLTK
pacman -Q mingw-w64-x86_64-fltk
ls /mingw64/bin/libfltk.dll

# 6. Run diagnostics
bash debug/diagnose_0xc000007b.sh

# 7. If diagnostics pass, rebuild
./build_windows.sh

# 8. Verify result
file build/windows/*.dll
file build/windows/Template.exe

# 9. Test
./build/windows/Template.exe
```

## Understanding the Error

**0xc000007b** means: "The application or DLL format is invalid"

This typically happens when Windows tries to load:
- 32-bit DLL for 64-bit process
- Corrupted DLL file
- DLL missing critical section or dependency

### Why It Happens

| Scenario | Result |
|----------|--------|
| 64-bit exe + 64-bit DLLs | ✅ Works |
| 64-bit exe + 32-bit DLLs | ❌ 0xc000007b error |
| 32-bit exe + 32-bit DLLs | ✅ Works |
| 32-bit exe + 64-bit DLLs | ❌ 0xc000007b error |

**Our config**: 64-bit exe + 64-bit DLLs only

## Advanced Diagnostics

### Check DLL Dependencies

```bash
# See what each DLL depends on (requires objdump)
x86_64-w64-mingw32-objdump -p build/windows/libfltk.dll | grep "DLL Name"
```

### Check for Dependency Loops

```bash
# Verify no circular dependencies
depends build/windows/libfltk.dll
```

(requires depends.exe from Visual Studio or separate tool)

## Still Getting 0xc000007b?

If you've completed all steps and still get the error:

1. **Check antivirus**: Temporarily disable to see if it's blocking DLLs
2. **Check file permissions**: Run as Administrator
3. **Check Event Viewer**: Look for system errors when exe crashes
4. **Alternative**: Deploy to different 64-bit Windows system to isolate the issue
5. **Last resort**: Extract build/windows/ folder contents and inspect for anomalies

## Prevention

For future builds:

```bash
# Always run diagnostics before building
bash DLL_DIAGNOSTICS.sh

# The build script now does this automatically, but you can also:
./build_windows.sh
# This will run diagnostics and abort if issues found
```

---

## Reference

- [MSYS2 Wiki - MinGW](https://wiki.archlinux.org/title/MinGW)
- [Windows Error Codes](https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes)
- [FLTK Documentation](https://www.fltk.org/doc-1.4/index.html)
