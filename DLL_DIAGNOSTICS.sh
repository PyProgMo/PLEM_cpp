#!/bin/bash
# DLL Architecture Compatibility Diagnostics
# Checks for 32-bit/64-bit mismatches and verifies DLL availability
# Run before building to identify compatibility issues

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}==================================================${NC}"
echo -e "${BLUE}DLL Architecture Compatibility Diagnostics${NC}"
echo -e "${BLUE}==================================================${NC}"
echo ""

# Function to check if a file is 32-bit or 64-bit
check_architecture() {
    local file="$1"
    local filename=$(basename "$file")
    
    if [ ! -f "$file" ]; then
        echo -e "${RED}✗ NOT FOUND${NC}: $filename"
        return 2
    fi
    
    local output=$(file "$file")
    
    if echo "$output" | grep -q "PE32+"; then
        return 0
    elif echo "$output" | grep -q "PE32"; then
        echo -e "${RED}✗ 32-bit${NC}: $filename (INCOMPATIBLE - should be 64-bit)"
        return 1
    else
        echo -e "${YELLOW}? UNKNOWN${NC}: $filename"
        return 2
    fi
}

# Search for the first matching DLL across candidate directories.
find_dll_match() {
    local pattern="$1"
    shift
    local dir
    local candidate

    for dir in "$@"; do
        shopt -s nullglob
        for candidate in "$dir"/$pattern; do
            echo "$candidate"
            shopt -u nullglob
            return 0
        done
        shopt -u nullglob
    done

    return 1
}

# Check MinGW64 Installation
echo -e "${BLUE}1. Checking MinGW64 Installation${NC}"
echo "   Compiler check..."

if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    COMPILER_PATH=$(which x86_64-w64-mingw32-g++)
    echo -e "   ${GREEN}✓ 64-bit MinGW compiler found${NC}: $COMPILER_PATH"
else
    echo -e "   ${RED}✗ 64-bit MinGW compiler NOT found${NC}"
    echo "   Please install: pacman -S mingw-w64-x86_64-gcc"
    EXIT_STATUS=1
fi

if command -v i686-w64-mingw32-g++ &> /dev/null; then
    echo -e "   ${YELLOW}⚠ 32-bit MinGW compiler found${NC} (not needed, but present)"
fi

echo ""

# Check FLTK Installation
echo -e "${BLUE}2. Checking FLTK Installation${NC}"

FLTK_FOUND=0
FLTK_PATH=""

# Check common FLTK installation paths
for path in /mingw64 /usr/local/opt/fltk /opt/fltk; do
    if [ -d "$path/bin" ] && [ -f "$path/bin/fltk-config" ]; then
        echo -e "   ${GREEN}✓ FLTK found at${NC}: $path"
        FLTK_PATH="$path"
        FLTK_FOUND=1
        break
    fi
done

if [ $FLTK_FOUND -eq 0 ]; then
    echo -e "   ${RED}✗ FLTK not found${NC}"
    echo "   Please install: pacman -S mingw-w64-x86_64-fltk"
    EXIT_STATUS=1
else
    # Check FLTK version
    FLTK_VERSION=$("$FLTK_PATH/bin/fltk-config" --version 2>/dev/null || echo "unknown")
    echo "   FLTK Version: $FLTK_VERSION"
fi

echo ""

# Check 64-bit FLTK DLLs
echo -e "${BLUE}3. Checking 64-bit FLTK Runtime DLLs${NC}"

if [ -d "$FLTK_PATH/bin" ] || [ -d "build/windows" ] || [ -d "build/dlls" ]; then
    DLL_SCAN_DIRS=("build/windows" "build/dlls")
    if [ -d "$FLTK_PATH/bin" ]; then
        DLL_SCAN_DIRS+=("$FLTK_PATH/bin")
    fi
    DLL_SCAN_DIRS+=("/mingw64/bin" "/mingw32/bin")
    echo "   Scanning: ${DLL_SCAN_DIRS[*]}"
    echo ""

    DLL_CHECK_FAILED=0

    check_pattern_architecture() {
        local label="$1"
        local pattern="$2"
        shift 2
        local match
        local found_64=0
        local found_any=0

        match=$(find_dll_match "$pattern" "$@") || true
        if [ -n "$match" ]; then
            found_any=1
            if check_architecture "$match"; then
                found_64=1
            fi
        fi

        if [ $found_any -eq 0 ]; then
            echo -e "${RED}✗ NOT FOUND${NC}: $label"
            DLL_CHECK_FAILED=1
        elif [ $found_64 -eq 0 ]; then
            echo -e "${RED}✗ No 64-bit match found${NC}: $label"
            DLL_CHECK_FAILED=1
        fi
    }

    # Check each required DLL or versioned DLL family
    check_pattern_architecture "libfltk.dll" "libfltk*.dll" "${DLL_SCAN_DIRS[@]}"
    check_pattern_architecture "libfltk_images.dll" "libfltk_images*.dll" "${DLL_SCAN_DIRS[@]}"
    check_pattern_architecture "libpng.dll" "libpng*.dll" "${DLL_SCAN_DIRS[@]}"
    check_pattern_architecture "libz.dll" "libz*.dll" "${DLL_SCAN_DIRS[@]}"
else
    echo -e "   ${RED}✗ Cannot find FLTK bin directory${NC}"
    EXIT_STATUS=1
fi

echo ""

# Check Thorlabs DLL
echo -e "${BLUE}4. Checking Thorlabs Power Meter DLL${NC}"

THORLABS_DLL="standalone_devices/thorlabs_powermeter/dll/TLPM_64.dll"
if [ -f "$THORLABS_DLL" ]; then
    check_architecture "$THORLABS_DLL" || EXIT_STATUS=1
else
    echo -e "   ${RED}✗ TLPM_64.dll not found${NC} at $THORLABS_DLL"
    EXIT_STATUS=1
fi

echo ""

# Check compiled executable (if it exists)
echo -e "${BLUE}5. Checking Compiled Executable${NC}"

if [ -f "build/windows/PLEMv5.exe" ]; then
    check_architecture "build/windows/PLEMv5.exe" || EXIT_STATUS=1
else
    echo -e "   ${YELLOW}⊘ Executable not yet built${NC}"
fi

echo ""

# Summary
echo -e "${BLUE}==================================================${NC}"
echo -e "${BLUE}Summary & Recommendations${NC}"
echo -e "${BLUE}==================================================${NC}"
echo ""

if [ $FLTK_FOUND -eq 0 ]; then
    echo -e "${RED}CRITICAL: FLTK not installed${NC}"
    echo "Install with: pacman -S mingw-w64-x86_64-fltk"
    echo "Ensure you select the 64-bit version (x86_64)"
    EXIT_STATUS=1
elif [ $DLL_CHECK_FAILED -eq 1 ]; then
    echo -e "${RED}ERROR: DLL compatibility issues detected${NC}"
    echo ""
    echo "Issues found:"
    echo "1. Some required DLLs are missing or not 64-bit"
    echo ""
    echo "Solutions:"
    echo "• Remove 32-bit FLTK: pacman -R mingw-w64-i686-fltk"
    echo "• Install 64-bit FLTK: pacman -S mingw-w64-x86_64-fltk"
    echo "• Verify installation: pacman -Q mingw-w64-x86_64-fltk"
    EXIT_STATUS=1
else
    echo -e "${GREEN}✓ All checks passed${NC}"
    echo ""
    echo "Configuration is compatible:"
    echo "• Compiler: 64-bit MinGW64"
    echo "• FLTK: 64-bit installed"
    echo "• DLLs: All 64-bit"
    echo "• Thorlabs DLL: 64-bit"
    echo ""
    echo "You can safely run: ./build_windows.sh"
    EXIT_STATUS=0
fi

echo ""
echo -e "${BLUE}==================================================${NC}"

exit $EXIT_STATUS
