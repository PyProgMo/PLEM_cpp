#!/bin/bash
#
# build_windows.sh - E816 NanoStage Controller Build Script
# For MSYS2 MinGW 32-bit on Windows
#
# Usage: bash build_windows.sh
# Or:    ./build_windows.sh
#

set -e  # Exit on error

if [ "${MSYSTEM:-}" != "MINGW32" ]; then
    echo "This script must be run from the MSYS2 MinGW 32-bit shell (mingw32.exe)."
    echo "Open 'MSYS2 MinGW 32-bit' and run: ./build_windows.sh"
    exit 1
fi

# ============================================================
# Configuration
# ============================================================

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
OBJ_DIR="${BUILD_DIR}/obj"
EXECUTABLE="${BUILD_DIR}/test.exe"
DLL_SOURCE="${PROJECT_DIR}/E816_DLL/E816_DLL.dll"
DLL_TARGET="${BUILD_DIR}/E816_DLL.dll"
PRESET_USB_ID="109021162"
MINGW32_BIN="/mingw32/bin"

if [ -x "${MINGW32_BIN}/gcc" ]; then
    GCC_BIN="${MINGW32_BIN}/gcc"
else
    GCC_BIN="$(command -v gcc)"
fi

if [ -x "${MINGW32_BIN}/make" ]; then
    MAKE_BIN="${MINGW32_BIN}/make"
else
    MAKE_BIN="$(command -v make)"
fi

if command -v gendef >/dev/null 2>&1; then
    GENDEF_BIN="$(command -v gendef)"
else
    GENDEF_BIN="${MINGW32_BIN}/gendef"
fi

if command -v dlltool >/dev/null 2>&1; then
    DLLTOOL_BIN="$(command -v dlltool)"
else
    DLLTOOL_BIN="${MINGW32_BIN}/dlltool"
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;36m'
NC='\033[0m'  # No Color

# ============================================================
# Helper Functions
# ============================================================

print_header() {
    echo ""
    echo "======================================================"
    echo "E816 NanoStage Controller - Build Script"
    echo "======================================================"
    echo ""
    echo "[*] Project directory: $PROJECT_DIR"
    echo ""
}

print_step() {
    echo -e "${BLUE}[$1/5]${NC} $2"
}

print_ok() {
    echo -e "${GREEN}[OK]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# ============================================================
# Step 1: Verify Tools
# ============================================================

verify_tools() {
    print_step "1" "Verifying compilation tools"
    echo ""
    
    if [ ! -x "${GCC_BIN}" ]; then
        print_error "gcc not found! Please install MinGW32 toolchain"
    fi
    print_ok "gcc found: $(${GCC_BIN} --version | head -1)"
    
    if [ ! -x "${MAKE_BIN}" ]; then
        print_warning "make not found - will use direct gcc commands"
        USE_MAKE=0
    else
        print_ok "make found: $(${MAKE_BIN} --version | head -1)"
        USE_MAKE=1
    fi
    
    if [ -x "${GENDEF_BIN}" ]; then
        print_ok "gendef found"
        HAS_GENDEF=1
    else
        print_warning "gendef not found - cannot generate import library"
        HAS_GENDEF=0
    fi
    
    if [ -x "${DLLTOOL_BIN}" ]; then
        print_ok "dlltool found"
        HAS_DLLTOOL=1
    else
        print_warning "dlltool not found - cannot generate import library"
        HAS_DLLTOOL=0
    fi
    
    echo ""
}

# ============================================================
# Step 2: Check Project Files
# ============================================================

verify_project_files() {
    print_step "2" "Verifying project files"
    echo ""
    
    if [ ! -f "${PROJECT_DIR}/src/main.c" ]; then
        print_error "src/main.c not found!"
    fi
    print_ok "src/main.c found"
    
    if [ ! -f "${PROJECT_DIR}/include/E816_DLL.h" ]; then
        print_error "include/E816_DLL.h not found!"
    fi
    print_ok "include headers found"
    
    if [ ! -f "${DLL_SOURCE}" ]; then
        print_error "E816_DLL/E816_DLL.dll not found!"
    fi
    print_ok "E816_DLL.dll found"
    
    echo ""
}

# ============================================================
# Step 3: Create Build Directories
# ============================================================

setup_directories() {
    print_step "3" "Setting up build directories"
    echo ""
    
    if [ ! -d "${BUILD_DIR}" ]; then
        mkdir -p "${BUILD_DIR}"
        print_ok "Created ${BUILD_DIR}"
    fi
    
    if [ ! -d "${OBJ_DIR}" ]; then
        mkdir -p "${OBJ_DIR}"
        print_ok "Created ${OBJ_DIR}"
    fi
    
    echo ""
}

# ============================================================
# Step 4: Generate Import Library (vendor DLL compatible)
# ============================================================
# IMPORTANT: Vendor E816_DLL.dll exports undecorated symbol names.
# We normalize gendef output by removing trailing @N suffixes so
# libPI_E816.a exposes undecorated imports matching runtime lookup.
# ============================================================

generate_import_library() {
    if [ ${HAS_GENDEF} -ne 1 ] || [ ${HAS_DLLTOOL} -ne 1 ]; then
        return
    fi
    
    IMPORT_LIB="${PROJECT_DIR}/E816_DLL/libPI_E816.a"
    
    echo "[*] Generating import library from DLL..."
    echo "    Using undecorated symbol names for vendor DLL compatibility"
    echo ""
    
    cd "${PROJECT_DIR}/E816_DLL"
    
    # Always regenerate to ensure consistency with DLL exports
    rm -f "E816_DLL.def" "E816_DLL.raw.def" "libPI_E816.a"

    # Run gendef to extract export table from DLL.
    # gendef writes E816_DLL.def directly to disk.
    if ! "${GENDEF_BIN}" ./E816_DLL.dll; then
        print_warning "gendef returned a non-zero exit code"
    fi

    # Check if def file was created and has content
    if [ ! -f "E816_DLL.def" ] || [ ! -s "E816_DLL.def" ]; then
        print_warning "Failed to generate E816_DLL.def"
        cd "${PROJECT_DIR}"
        return
    fi

    cp "E816_DLL.def" "E816_DLL.raw.def"

    # Normalize export names (remove stdcall suffixes @N)
    if ! sed -E 's/@[0-9]+$//' "E816_DLL.raw.def" > "E816_DLL.def"; then
        print_warning "Failed to normalize export names"
        cd "${PROJECT_DIR}"
        return
    fi

    if [ ! -f "E816_DLL.def" ] || [ ! -s "E816_DLL.def" ]; then
        print_warning "Failed to generate normalized E816_DLL.def"
        cd "${PROJECT_DIR}"
        return
    fi

    print_ok "Generated normalized E816_DLL.def (undecorated exports)"
    
    # Generate import library with normalized symbols
    if ! "${DLLTOOL_BIN}" -d E816_DLL.def -l libPI_E816.a > /dev/null 2>&1; then
        print_warning "Failed to generate import library"
        cd "${PROJECT_DIR}"
        return
    fi
    
    cd "${PROJECT_DIR}"
    
    if [ -f "${IMPORT_LIB}" ]; then
        print_ok "Import library generated: ${IMPORT_LIB}"
    else
        print_warning "Import library not created"
    fi
    
    echo ""
}

# ============================================================
# Step 5: Compile and Link
# ============================================================

compile_project() {
    print_step "4" "Compiling and linking"
    echo ""
    
    # Always regenerate import library so symbols stay aligned with vendor DLL exports
    generate_import_library
    
    if [ ${USE_MAKE} -eq 1 ]; then
        echo "[*] Using make for compilation..."
        echo ""
        
        cd "${PROJECT_DIR}"
        make clean > /dev/null 2>&1 || true
        
        if ! "${MAKE_BIN}"; then
            print_error "Build failed with make!"
        fi
        
        print_ok "Build completed successfully"
    else
        echo "[*] Using direct gcc compilation..."
        echo ""
        
        # Compile with undecorated import declarations (from headers)
        echo "[*] Compiling src/main.c..."
        if ! "${GCC_BIN}" -B/mingw32/bin -Wall -O2 -std=c99 \
            -I"${PROJECT_DIR}/include" \
            -I"${PROJECT_DIR}/E816_DLL" \
            -c "${PROJECT_DIR}/src/main.c" \
            -o "${OBJ_DIR}/main.o"; then
            print_error "Compilation failed!"
        fi
        print_ok "Compilation successful"
        
        # Link - linker will find undecorated symbols in libPI_E816.a
        echo "[*] Linking..."
        if ! "${GCC_BIN}" -B/mingw32/bin "${OBJ_DIR}/main.o" \
            -L"${PROJECT_DIR}/E816_DLL" \
            -lPI_E816 \
            -o "${EXECUTABLE}"; then
            print_error "Linking failed!"
        fi
        print_ok "Linking successful"
    fi
    
    echo ""
}

# ============================================================
# Step 6: Copy DLL to Build Directory
# ============================================================

copy_dll() {
    print_step "5" "Finalizing build"
    echo ""
    
    if [ ! -f "${DLL_SOURCE}" ]; then
        print_warning "DLL source not found at ${DLL_SOURCE}"
        return
    fi
    
    if ! cp "${DLL_SOURCE}" "${DLL_TARGET}"; then
        print_warning "Failed to copy DLL to build directory"
        return
    fi
    
    print_ok "DLL copied to ${DLL_TARGET}"
}

# ============================================================
# Step 7: Verify Build Output
# ============================================================

verify_build() {
    echo ""
    
    if [ ! -f "${EXECUTABLE}" ]; then
        print_error "Executable not created at ${EXECUTABLE}"
    fi
    print_ok "Executable created: ${EXECUTABLE}"
    
    if [ ! -f "${DLL_TARGET}" ]; then
        print_warning "DLL not copied to build directory"
    else
        print_ok "DLL present in build directory"
    fi
    
    echo ""
}

try_preset_usb_connection() {
    echo ""
    echo "[*] Trying preset USB connection with ID: ${PRESET_USB_ID}"

    if [ ! -f "${EXECUTABLE}" ]; then
        print_warning "Skipping preset USB check; executable not found"
        return
    fi

    cd "${BUILD_DIR}"
    if ./test.exe --connect-usb-id "${PRESET_USB_ID}"; then
        print_ok "Preset USB connection test succeeded"
    else
        print_warning "Preset USB connection test failed for ID ${PRESET_USB_ID}"
    fi
    cd "${PROJECT_DIR}"
}

# ============================================================
# Main Script
# ============================================================

run_executable() {
    echo ""
    echo "[*] Running ${EXECUTABLE}..."
    echo ""

    if [ ! -f "${EXECUTABLE}" ]; then
        print_error "Executable not found: ${EXECUTABLE}"
    fi

    # Run the Windows executable directly from MSYS2.
    cd "${BUILD_DIR}"
    ./test.exe
    local rc=$?

    if [ ${rc} -ne 0 ]; then
        echo ""
        print_warning "Executable returned non-zero exit code: ${rc}"
        if [ ${rc} -eq 53 ]; then
            print_warning "Possible cause: DLL entry point not found (Windows STATUS_ENTRYPOINT_NOT_FOUND)."
        fi
        echo ""
        echo "[*] Quick checks:"
        echo "    - Ensure ${DLL_TARGET} exists"
        echo "    - Run from build dir: cd ${BUILD_DIR} && ./test.exe"
        echo ""
    fi
}

main() {
    print_header
    
    verify_tools
    verify_project_files
    setup_directories
    compile_project
    copy_dll
    verify_build
    try_preset_usb_connection
    
    # Success message
    echo "======================================================"
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo "======================================================"
    echo ""
    echo "[*] Executable: ${EXECUTABLE}"
    echo "[*] Output directory: ${BUILD_DIR}"
    echo ""
    
    # Ask what to do next
    echo "Options:"
    echo "  1 - Run the executable"
    echo "  2 - Open build directory"
    echo "  3 - Exit"
    echo ""
    read -p "Choose an option (1-3): " choice
    
    case $choice in
        1)
            run_executable
            ;;
        2)
            echo "[*] Opening build directory..."
            start explorer "${BUILD_DIR}" 2>/dev/null || echo "Cannot open explorer from MSYS2"
            ;;
        *)
            echo ""
            echo "[*] To run manually, execute:"
            echo "    cd ${BUILD_DIR}"
            echo "    ./test.exe"
            ;;
    esac
}

# Run main function
main
