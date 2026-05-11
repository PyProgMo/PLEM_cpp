# E816 NanoStage Controller - Build Instructions
## MinGW32 (MSYS2) Project

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Project Structure](#project-structure)
3. [Setup](#setup)
4. [Building](#building)
5. [Running](#running)
6. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Software
- **MSYS2** - Download from: https://www.msys2.org/
- **MinGW32** - Included with MSYS2 (32-bit gcc toolchain)
- **Required tools**: `gcc`, `make`, `gendef`, `dlltool`

### Installation Steps

1. **Install MSYS2**
   - Download MSYS2 installer from https://www.msys2.org/
   - Run the installer and follow the prompts
   - Default installation path: `C:\msys64`

2. **Update MSYS2 Package Manager**
   ```bash
   pacman -Syu
   ```
   - Answer `Y` to any prompts
   - The terminal may close; reopen it and run the command again until there are no more updates

3. **Install MinGW32 Toolchain**
   ```bash
   pacman -S mingw-w64-i686-toolchain mingw-w64-i686-make
   ```
   - When prompted, press `Enter` to install all packages in the group
   - This installs: `gcc`, `binutils`, `make`, etc.

4. **Verify Installation**
   Open "MSYS2 MinGW 32-bit" shell and verify:
   ```bash
   gcc --version      # Should show MinGW 32-bit gcc
   make --version     # Should show make
   gendef --version   # Should show binutils version (contains gendef)
   dlltool --version  # Should show binutils version
   ```

---

## Project Structure

```
nanostage/
├── Makefile                    # Build configuration
├── BUILD_INSTRUCTIONS.md       # This file
├── E816_DLL/                   # DLL files from manufacturer
│   ├── E816_DLL.dll           # The main DLL
│   ├── E816_DLL.lib           # Visual Studio import library (not used)
│   ├── libPI_E816.a           # MinGW import library (generated if needed)
│   ├── PI_E816.def            # DLL definition file (generated if needed)
│   ├── E816_DLL.h             # Header file (C interface)
│   └── picontrollererrors.h   # Error code definitions
├── include/                    # Header files for compilation
│   ├── E816_DLL.h             # Copy of manufacturer header
│   └── picontrollererrors.h   # Copy of error definitions
├── src/                        # Source files
│   └── main.c                 # Example/test program
└── build/                      # Build output directory (created by make)
    ├── test.exe               # Compiled executable
    ├── E816_DLL.dll           # DLL (copied here for runtime)
    └── obj/                   # Object files
        └── main.o
```

---

## Setup

### 1. Open MSYS2 MinGW 32-bit Shell

**On Windows:**
- Start Menu → Search "MSYS2 MinGW 32-bit"
- Or run: `C:\msys64\mingw32.exe`

**Note**: Always use the "MinGW 32-bit" shell, NOT the "MSYS2 MSYS" shell.

### 2. Navigate to Project Directory

```bash
cd /c/Users/PLM_104/Desktop/PLEMv5/standalone_devices/nanostage
```

Or if using a different path:
```bash
cd /path/to/nanostage
```

### 3. Verify Directory Structure

```bash
ls -la
```

You should see:
- `Makefile`
- `src/` directory with `main.c`
- `include/` directory with headers
- `E816_DLL/` directory with the DLL and headers

---

## Building

### Option 1: Quick Build (Recommended)

```bash
make
```

This will:
1. Check if `E816_DLL.dll` exists
2. Create directories (`build/`, `build/obj/`)
3. Generate the import library if needed (using `gendef` and `dlltool`)
4. Compile `src/main.c` → `build/obj/main.o`
5. Link the executable → `build/test.exe`
6. Copy `E816_DLL.dll` to `build/`

### Option 2: Step-by-Step Build

If you need more control:

```bash
# Step 1: Generate import library (if not already done)
make generate-lib

# Step 2: Compile only
make build-dirs
gcc -Wall -O2 -std=c99 -Iinclude -IE816_DLL -c src/main.c -o build/obj/main.o

# Step 3: Link
gcc build/obj/main.o -LE816_DLL -lPI_E816 -o build/test.exe

# Step 4: Copy DLL
cp E816_DLL/E816_DLL.dll build/
```

### Option 3: Manual Build (Educational)

If `make` is not available or you want to understand the process:

```bash
# Generate import library from DLL
gendef E816_DLL/E816_DLL.dll
dlltool -d PI_E816.def -l E816_DLL/libPI_E816.a
mv PI_E816.def E816_DLL/

# Create build directories
mkdir -p build/obj

# Compile
gcc -Wall -O2 -std=c99 -Iinclude -IE816_DLL -c src/main.c -o build/obj/main.o

# Link
gcc build/obj/main.o -LE816_DLL -lPI_E816 -o build/test.exe

# Copy DLL
cp E816_DLL/E816_DLL.dll build/
```

---

## Running

### Execute the Test Program

```bash
make run
```

Or directly:

```bash
cd build
./test.exe
```

### Expected Output

If the device is connected:
```
========================================
E816 Nanoscale Stage Controller - Test
========================================

[1] Attempting USB connection...
    Connection successful! Device ID: 1

[3] Verifying connection...
    Device is connected and responsive.

[4] Querying device identification...
    Device ID: E-816 NanoStage Controller v2.0
    ...
```

If the device is not connected:
```
[1] Attempting USB connection...
    USB connection failed (ID: -1)

[2] Attempting RS232 connection on COM1 (9600 baud)...
    RS232 connection failed (ID: -1)

[ERROR] Could not establish connection to device.
    Make sure the E816 controller is:
    - Connected via USB or RS232
    - Powered on
    - Properly installed
```

---

## Troubleshooting

### Issue: "gcc: command not found"

**Solution:**
- Ensure you're using the "MSYS2 MinGW 32-bit" shell
- Run the installation command again: `pacman -S mingw-w64-i686-toolchain`

### Issue: "make: command not found"

**Solution:**
- Install make: `pacman -S mingw-w64-i686-make`
- Or use: `pacman -S make`

### Issue: "gendef/dlltool: command not found"

**Solution:**
- These come with binutils, part of the toolchain
- Reinstall: `pacman -S mingw-w64-i686-binutils`

### Issue: "libPI_E816.a not found during linking"

**Solution:**
- Regenerate the import library:
  ```bash
  make generate-lib
  ```
- Or manually:
  ```bash
  cd E816_DLL
  gendef E816_DLL.dll
  dlltool -d PI_E816.def -l libPI_E816.a
  ```

### Issue: "E816_DLL.dll not found"

**Solution:**
- Verify `E816_DLL/E816_DLL.dll` exists
- Copy it to the correct location if needed
- Check file permissions: `ls -la E816_DLL/`

### Issue: "undefined reference to 'E816_ConnectUSB'"

**Solution:**
- The DLL linking is not working
- Check that `libPI_E816.a` exists: `ls -la E816_DLL/libPI_E816.a`
- Try regenerating it: `make generate-lib`
- Ensure the import library was created correctly:
  ```bash
  nm E816_DLL/libPI_E816.a | grep -i E816_ConnectUSB
  ```

### Issue: "The application failed to initialize properly (0xc0000135)"

**Solution:**
- Missing or incompatible DLL at runtime
- Ensure `E816_DLL.dll` is in the same directory as `test.exe`
- Check the build directory: `ls -la build/`
- Copy manually if needed: `cp E816_DLL/E816_DLL.dll build/`

### Issue: Device connection fails

**Solution (for testing without hardware):**
- The program is designed to fail gracefully when hardware is not available
- To test compilation without hardware:
  1. Build the project: `make`
  2. The executable will be created even without the device connected
  3. Running it will show the connection error (which is normal)

---

## Common Make Targets

```bash
make              # Build everything (default)
make build        # Alias for 'make' (build everything)
make run          # Build and run test.exe
make clean        # Remove all build artifacts
make generate-lib # Generate import library from DLL
make help         # Show help message
make info         # Show project configuration
```

---

## Project Compilation Details

### Compiler Command

```bash
gcc -Wall -O2 -std=c99 -Iinclude -IE816_DLL -c src/main.c -o build/obj/main.o
```

- `-Wall`: Enable all warnings
- `-O2`: Level 2 optimization (good balance)
- `-std=c99`: Use C99 standard
- `-Iinclude`: Include headers from `./include`
- `-IE816_DLL`: Include headers from `./E816_DLL`

### Linker Command

```bash
gcc build/obj/main.o -LE816_DLL -lPI_E816 -o build/test.exe
```

- `-LE816_DLL`: Search for libraries in `./E816_DLL`
- `-lPI_E816`: Link with `libPI_E816.a` (the import library)
- `-o build/test.exe`: Output executable

---

## Advanced: Customizing the Build

### Change Optimization Level

Edit `Makefile` and change `CFLAGS`:
```makefile
# For debugging (no optimization, include debug symbols):
CFLAGS = -Wall -g -std=c99

# For maximum performance:
CFLAGS = -Wall -O3 -march=native -std=c99
```

### Add More Source Files

Edit `Makefile`:
```makefile
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/util.c
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/util.o
```

### Change Output Directory

Edit `Makefile`:
```makefile
BUILD_DIR = ../output  # Different location
```

---

## Next Steps

1. **Build the project**: `make`
2. **Connect the E816 device** via USB or RS232
3. **Run the test**: `make run`
4. **Examine the code**: Review `src/main.c` to understand the API
5. **Extend the example**: Add more DLL calls as needed for your application

---

## References

- **PI E816 Documentation**: Provided by Physik Instrumente
- **MSYS2 Documentation**: https://www.msys2.org/
- **MinGW Documentation**: https://www.mingw-w64.org/
- **GCC Manual**: https://gcc.gnu.org/onlinedocs/

---

## Support

For issues with:
- **MSYS2 Setup**: See https://www.msys2.org/docs/
- **E816 DLL**: Contact Physik Instrumente support
- **MinGW/GCC**: See https://www.mingw-w64.org/

---

**Last Updated**: May 5, 2026
**Project**: E816 NanoStage Controller - MinGW32 (MSYS2)
