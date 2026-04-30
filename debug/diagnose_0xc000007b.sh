#!/bin/bash
# Runtime dependency diagnostics for Windows 0xc000007b failures.
# Focuses on the exe plus DLLs in build/windows and build/dlls.

set -u

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

EXE_PATH="${1:-build/windows/Template.exe}"
RUNTIME_DIRS=("build/windows" "build/dlls")
DEV_DIRS=("/mingw64/bin" "/mingw32/bin")
SCAN_DIRS=("${RUNTIME_DIRS[@]}" "${DEV_DIRS[@]}")
IGNORED_DLLS=(
    api-ms-win-*.dll
    ext-ms-win-*.dll
    kernel32.dll
    user32.dll
    gdi32.dll
    comdlg32.dll
    comctl32.dll
    ole32.dll
    uuid.dll
    ws2_32.dll
    shell32.dll
    advapi32.dll
    winspool.dll
    winspool.drv
    gdiplus.dll
    msvcrt.dll
    ucrtbase.dll
    bluetoothapis.dll
    setupapi.dll
    version.dll
    cfgmgr32.dll
    oleaut32.dll
    rpcrt4.dll
    secur32.dll
    shlwapi.dll
)

OBJUMP_CMD=""
for candidate in \
    x86_64-w64-mingw32-objdump \
    objdump \
    /c/msys64/mingw64/bin/objdump.exe \
    /c/msys64/usr/bin/objdump.exe \
    /c/Program\ Files/Git/usr/bin/objdump.exe; do
    if command -v "$candidate" >/dev/null 2>&1; then
        OBJUMP_CMD="$candidate"
        break
    fi
    if [ -x "$candidate" ]; then
        OBJUMP_CMD="$candidate"
        break
    fi
done

if [ -z "$OBJUMP_CMD" ]; then
    echo -e "${RED}ERROR${NC}: objdump not found"
    echo "Install binutils or run this from an MSYS2 MinGW64 shell."
    exit 1
fi

if [ ! -f "$EXE_PATH" ]; then
    echo -e "${RED}ERROR${NC}: exe not found: $EXE_PATH"
    exit 1
fi

if ! command -v file >/dev/null 2>&1; then
    echo -e "${RED}ERROR${NC}: file command not found"
    exit 1
fi

normalize_name() {
    local name="$1"
    echo "${name,,}"
}

is_ignored_dll() {
    local dll_name
    dll_name=$(normalize_name "$1")
    local pattern
    for pattern in "${IGNORED_DLLS[@]}"; do
        case "$dll_name" in
            $pattern) return 0 ;;
        esac
    done
    return 1
}

check_bitness() {
    local path="$1"
    local name
    name=$(basename "$path")

    if [ ! -f "$path" ]; then
        echo -e "${RED}MISSING${NC} $name"
        return 2
    fi

    local info
    info=$(file "$path")
    if echo "$info" | grep -q "PE32+"; then
        return 0
    fi

    if echo "$info" | grep -q "PE32"; then
        echo -e "${RED}32-BIT${NC} $name -> $path"
        return 1
    fi

    echo -e "${YELLOW}UNKNOWN${NC} $name -> $path"
    return 3
}

find_dll() {
    local dll_name="$1"
    local dir
    for dir in "${RUNTIME_DIRS[@]}"; do
        if [ -f "$dir/$dll_name" ]; then
            echo "$dir/$dll_name"
            return 0
        fi
    done
    return 1
}

find_any_dll() {
    local dll_name="$1"
    local dir
    for dir in "${SCAN_DIRS[@]}"; do
        if [ -f "$dir/$dll_name" ]; then
            echo "$dir/$dll_name"
            return 0
        fi
    done
    return 1
}

get_imports() {
    local path="$1"
    "$OBJUMP_CMD" -p "$path" 2>/dev/null | awk '/DLL Name:/{print $3}' | tr -d '\r' | sort -u
}

declare -A VISITED

inspect_file() {
    local path="$1"
    local label="$2"
    local chain="$3"

    if [ -n "${VISITED["$path"]+x}" ]; then
        return 0
    fi
    VISITED["$path"]=1

    local info
    info=$(file "$path")
    if ! echo "$info" | grep -q "PE32+"; then
        if echo "$info" | grep -q "PE32"; then
            echo -e "${RED}32-BIT${NC} $chain -> $path"
            return 1
        fi
        echo -e "${YELLOW}UNKNOWN${NC} $chain -> $path"
        return 3
    fi

    local imports
    imports=$(get_imports "$path")
    local bad=0

    while IFS= read -r import; do
        [ -z "$import" ] && continue

        if is_ignored_dll "$import"; then
            continue
        fi

        local resolved
        if resolved=$(find_dll "$import"); then
            if ! inspect_file "$resolved" "$import" "$chain -> $import"; then
                bad=1
            fi
        else
            if resolved=$(find_any_dll "$import"); then
                echo -e "${RED}DEV-ONLY${NC} $chain -> $import -> $resolved"
            else
                echo -e "${RED}MISSING${NC} $chain -> $import"
            fi
            bad=1
        fi
    done <<< "$imports"

    return $bad
}

echo -e "${BLUE}0xc000007b Runtime Diagnostics${NC}"
echo "Scanning: ${SCAN_DIRS[*]}"
echo "Target: $EXE_PATH"
echo ""

EXIT_STATUS=0

if ! inspect_file "$EXE_PATH" "Template.exe" "Template.exe"; then
    EXIT_STATUS=1
fi

if [ $EXIT_STATUS -eq 0 ]; then
    echo -e "${GREEN}OK${NC} No missing or 32-bit DLLs found in build/windows or build/dlls."
else
    echo ""
    echo -e "${YELLOW}Fix the failing DLLs above, then rebuild and rerun this script.${NC}"
fi

exit $EXIT_STATUS