#!/bin/bash
# Build script for Windows (MSYS2 MinGW 64-bit)
# Requires FLTK installed via MSYS2: pacman -S mingw-w64-x86_64-fltk

# Configuration
# Set to 'true' to start the executable automatically after a successful build
START_AFTER_BUILD=true

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

COMPILER="x86_64-w64-mingw32-g++"
if ! command -v $COMPILER &> /dev/null; then
    echo -e "${RED}ERROR: MinGW 64-bit compiler not found.${NC}"
    echo "Please run this in MSYS2 MinGW 64-bit terminal."
    echo "Verify with: x86_64-w64-mingw32-g++ --version"
    exit 1
fi

# Pre-build compatibility check
echo -e "${BLUE}Running pre-build compatibility check...${NC}"

# Parse arguments: allow --force or -f to continue even if diagnostics fail
FORCE=false
while [ "$#" -gt 0 ]; do
    case "$1" in
        --force|-f)
            FORCE=true
            shift
            ;;
        *)
            # ignore unknown args to keep compatibility
            shift
            ;;
    esac
done

if [ -f "DLL_DIAGNOSTICS.sh" ]; then
    bash DLL_DIAGNOSTICS.sh
    DIAG_EXIT=$?
    if [ $DIAG_EXIT -ne 0 ]; then
        if [ "$FORCE" = true ]; then
            echo -e "${YELLOW}Compatibility check failed, but --force specified; continuing build with warnings.${NC}"
        else
            echo -e "${RED}Compatibility check failed. Aborting build.${NC}"
            exit 1
        fi
    fi
else
    echo -e "${YELLOW}Warning: DLL_DIAGNOSTICS.sh not found. Skipping compatibility check.${NC}"
fi
echo ""

if tasklist 2>/dev/null | grep -q "Template.exe"; then
    echo "Closing running Template.exe to avoid file lock..."
    taskkill -F -IM Template.exe >/dev/null 2>&1
    sleep 1
fi

echo "Building FLTK Template..."
$COMPILER -std=c++17 \
    -Iinclude \
    -I/mingw64/include \
    -Istandalone_devices/thorlabs_powermeter/dll \
    -Istandalone_devices/thorlabs_powermeter/Include \
    src/main.cpp \
    src/backend/DebugConsole.cpp \
    src/backend/AutoStage.cpp \
    src/backend/AndorBackend.cpp \
    standalone_devices/andorwrapper/cpp/AndorCCD.cpp \
    src/fbconnector/FBConnector.cpp \
    standalone_devices/thorlabs_powermeter/tl100d_reader/src/ThorlabsPM.cpp \
    -o build/windows/Template.exe \
    -mwindows \
    -L/mingw64/lib \
    -Lstandalone_devices/thorlabs_powermeter/dll \
    -Lstandalone_devices/andorwrapper/pytest \
    -lTLPM_64 \
    -latmcd64d \
    -lfltk_images -lpng -lz -lfltk -lgdi32 -lcomdlg32 -lcomctl32 -lole32 -luuid -lws2_32 -lkernel32 -luser32 -lgdiplus -lshell32 -lwinspool -ladvapi32 -static-libgcc

if [ -f build/windows/Template.exe ]; then
    echo "Build successful! Executable: build/windows/Template.exe"
    echo ""
    
    # Find FLTK installation path (best-effort, do not abort here if --force)
    echo -e "${BLUE}Detecting FLTK installation...${NC}"
    FLTK_BIN=""
    
    # Try common FLTK paths
    for path in /mingw64 /usr/local/opt/fltk /opt/fltk; do
        shopt -s nullglob
        fltk_candidates=("$path"/bin/libfltk*.dll)
        shopt -u nullglob
        if [ ${#fltk_candidates[@]} -gt 0 ]; then
            FLTK_BIN="$path/bin"
            echo -e "${GREEN}✓ Found FLTK at:${NC} $path"
            break
        fi
    done
    
    if [ -z "$FLTK_BIN" ]; then
        echo -e "${YELLOW}Warning: FLTK libraries not found in standard locations${NC}"
        echo "Expected: /mingw64/bin/ (64-bit)"
        echo "Continuing with best-effort search (use --force to override diagnostics)."
    fi
    
    echo ""
    echo -e "${BLUE}Copying 64-bit DLLs (searching preferred directories)...${NC}"

    # Candidate directories in priority order (exe dir first).
    # Also include local build/dlls and the workspace build/dlls absolute path as requested.
    CANDIDATE_DIRS=("build/windows" "build/dlls" "E:/Projects/plm_VI/hsimeasurement/build/dlls")
    if [ -n "$FLTK_BIN" ]; then
        CANDIDATE_DIRS+=("$FLTK_BIN")
    fi
    CANDIDATE_DIRS+=("/mingw64/bin" "/mingw32/bin")

    # Map to record which source was used for each DLL
    declare -A SRC_MAP

    # Function to check and copy DLL with architecture verification (keeps existing behavior)
    copy_dll_safe() {
        local source="$1"
        local dest="$2"
        local dll_name=$(basename "$source")

        if [ ! -f "$source" ]; then
            return 2
        fi

        # Check if DLL is 64-bit
        if file "$source" | grep -q "PE32+"; then
            cp "$source" "$dest" 2>/dev/null
            if [ -f "$dest/$dll_name" ]; then
                return 0
            else
                echo -e "${RED}✗ Copy failed:${NC} $dll_name"
                return 1
            fi
        elif file "$source" | grep -q "PE32"; then
            echo -e "${RED}✗ INCOMPATIBLE (32-bit):${NC} $dll_name  at $source"
            return 3
        else
            echo -e "${YELLOW}? UNKNOWN FORMAT:${NC} $dll_name  at $source"
            return 4
        fi
    }

    # Find a DLL in candidate dirs and copy it (records source)
    find_and_copy_dll() {
        local dll_name="$1"
        local dest="$2"
        local found=1
        for dir in "${CANDIDATE_DIRS[@]}"; do
            src="$dir/$dll_name"
            copy_dll_safe "$src" "$dest"
            case $? in
                0)
                    SRC_MAP["$dll_name"]="$src"
                    found=0
                    break
                    ;;
                2)
                    # not found in this dir, continue
                    ;;
                3)
                    # incompatible 32-bit found; record and continue searching
                    SRC_MAP["$dll_name"]="$src (32-bit)"
                    found=2
                    ;;
                *)
                    ;;
            esac
        done
        return $found
    }

    # Find by pattern (e.g., libfltk*.dll) in candidate dirs and copy first match
    find_and_copy_pattern() {
        local pattern="$1"
        local dest="$2"
        local name_label="$3" # key to store in SRC_MAP
        local found=1
        for dir in "${CANDIDATE_DIRS[@]}"; do
            shopt -s nullglob
            for candidate in "$dir"/$pattern; do
                # candidate is full path
                copy_dll_safe "$candidate" "$dest"
                case $? in
                    0)
                        SRC_MAP["$name_label"]="$candidate"
                        found=0
                        break 2
                        ;;
                    3)
                        SRC_MAP["$name_label"]="$candidate (32-bit)"
                        found=2
                        # continue searching for 64-bit
                        ;;
                    *)
                        ;;
                esac
            done
            shopt -u nullglob
        done
        return $found
    }

    # Copy project DLLs (prefer build/windows then project path)
    echo ""
    echo "Project DLLs:"
    # Add project dll source to candidates temporarily
    CANDIDATE_DIRS=("build/windows" "standalone_devices/thorlabs_powermeter/dll" "${CANDIDATE_DIRS[@]}")
    find_and_copy_dll "TLPM_64.dll" "build/windows" || true

    # Copy FLTK runtime DLLs
    echo ""
    echo "FLTK Runtime DLLs (searched dirs): ${CANDIDATE_DIRS[*]}"
    FLTK_COPY_SUCCESS=0
    FLTK_COPY_FAILED=0

    # Use pattern searches to match versioned filenames (e.g., libfltk-1.4.dll)
    find_and_copy_pattern "libfltk*.dll" "build/windows" "libfltk.dll"
    rc=$?
    if [ $rc -eq 0 ]; then FLTK_COPY_SUCCESS=$((FLTK_COPY_SUCCESS+1)); elif [ $rc -eq 2 ]; then FLTK_COPY_FAILED=$((FLTK_COPY_FAILED+1)); fi

    find_and_copy_pattern "libfltk_images*.dll" "build/windows" "libfltk_images.dll"
    rc=$?
    if [ $rc -eq 0 ]; then FLTK_COPY_SUCCESS=$((FLTK_COPY_SUCCESS+1)); elif [ $rc -eq 2 ]; then FLTK_COPY_FAILED=$((FLTK_COPY_FAILED+1)); fi

    # Handle libpng (could be libpng16.dll or libpng.dll or libpng-1.6.dll)
    find_and_copy_pattern "libpng*.dll" "build/windows" "libpng.dll"
    rc=$?
    if [ $rc -eq 0 ]; then FLTK_COPY_SUCCESS=$((FLTK_COPY_SUCCESS+1)); else FLTK_COPY_FAILED=$((FLTK_COPY_FAILED+1)); fi

    find_and_copy_pattern "zlib*.dll" "build/windows" "zlib1.dll"
    rc=$?
    if [ $rc -eq 0 ]; then FLTK_COPY_SUCCESS=$((FLTK_COPY_SUCCESS+1)); elif [ $rc -eq 2 ]; then FLTK_COPY_FAILED=$((FLTK_COPY_FAILED+1)); fi

    find_and_copy_pattern "libz*.dll" "build/windows" "libz.dll"
    rc=$?
    if [ $rc -eq 0 ]; then FLTK_COPY_SUCCESS=$((FLTK_COPY_SUCCESS+1)); elif [ $rc -eq 2 ]; then FLTK_COPY_FAILED=$((FLTK_COPY_FAILED+1)); fi

    if [ $FLTK_COPY_FAILED -gt 0 ]; then
        echo ""
        echo -e "${YELLOW}⚠ DLL copy issues detected${NC}"
        echo "The application may not run properly."
        echo ""
        echo "DLL source locations used (failures only):"
        for name in TLPM_64.dll libfltk.dll libfltk_images.dll libpng16.dll libpng.dll zlib1.dll libz.dll; do
            src=${SRC_MAP["$name"]}
            if [ -n "$src" ]; then
                echo " - $name -> $src"
            fi
        done
        echo ""
        echo "Troubleshooting:"
        echo "1. Verify 64-bit FLTK is installed:" 
        echo "   pacman -Q mingw-w64-x86_64-fltk"
        echo ""
        echo "2. If not installed, install it:"
        echo "   pacman -S mingw-w64-x86_64-fltk"
        echo ""
        echo "3. If 32-bit version is present, remove it:"
        echo "   pacman -R mingw-w64-i686-fltk"
        echo ""
        echo "4. Run diagnostics again:" 
        echo "   bash DLL_DIAGNOSTICS.sh"
    else
        echo -e "${GREEN}✓ DLL deployment OK (${FLTK_COPY_SUCCESS} runtime DLLs copied)${NC}"
    fi

    echo ""
    echo -e "${BLUE}Verifying deployment...${NC}"
    
    echo ""
    # Post-copy verification: ensure copied DLLs are 64-bit (PE32+)
    verify_copied_dlls() {
        local BAD=0
        local patterns=("TLPM_64.dll" "libfltk*.dll" "libfltk_images*.dll" "libpng*.dll" "libz*.dll")
        echo -e "${BLUE}Verifying copied DLL architectures in build/windows/...${NC}"
        for pat in "${patterns[@]}"; do
            shopt -s nullglob
            files=(build/windows/$pat)
            shopt -u nullglob
            if [ ${#files[@]} -eq 0 ]; then
                echo -e "${RED}✗ Missing:${NC} $pat"
                BAD=1
                continue
            fi
            # At least one match must be 64-bit
            local ok=0
            for f in "${files[@]}"; do
                if file "$f" | grep -q "PE32+"; then
                    ok=1
                    break
                fi
            done
            if [ $ok -eq 0 ]; then
                echo -e "${RED}✗ No 64-bit DLL found for pattern:${NC} $pat"
                BAD=1
            fi
        done

        if [ $BAD -ne 0 ]; then
            if [ "$FORCE" = true ]; then
                echo -e "${YELLOW}Compatibility issues detected but --force used; continuing with warnings.${NC}"
                return 0
            else
                echo -e "${RED}ERROR: Incompatible or missing DLLs detected. Aborting run.${NC}"
                exit 1
            fi
        fi
        if [ $BAD -eq 0 ]; then
            echo -e "${GREEN}✓ DLL verification passed${NC}"
        fi
        return 0
    }

    verify_copied_dlls
    
    if [ "$START_AFTER_BUILD" = true ]; then
        echo -e "${BLUE}Starting application...${NC}"
        EXE_PATH="build/windows/Template.exe"
        if command -v cygpath >/dev/null 2>&1 && command -v powershell.exe >/dev/null 2>&1; then
            EXE_WIN_PATH=$(cygpath -w "$EXE_PATH")
            WORKDIR_WIN_PATH=$(cygpath -w "$PWD")
            powershell.exe -NoProfile -Command "Start-Process -FilePath '$EXE_WIN_PATH' -WorkingDirectory '$WORKDIR_WIN_PATH'" >/dev/null 2>&1
        else
            ./build/windows/Template.exe &
        fi
    fi
else
    echo -e "${RED}Build failed. Check error messages above.${NC}"
    exit 1
fi
