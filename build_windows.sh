#!/bin/bash
# Build script for Windows (MSYS2 MinGW 64-bit)
# Requires FLTK installed via MSYS2: pacman -S mingw-w64-x86_64-fltk

# Configuration
# Set to 'true' to start the executable automatically after a successful build
START_AFTER_BUILD=true

COMPILER="x86_64-w64-mingw32-g++"
if ! command -v $COMPILER &> /dev/null; then
    echo "MinGW 64-bit compiler not found. Please run this in MSYS2 MinGW 64-bit terminal."
    exit 1
fi

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
    standalone_devices/thorlabs_powermeter/tl100d_reader/src/ThorlabsPM.cpp \
    -o build/windows/Template.exe \
    -mwindows \
    -L/mingw64/lib \
    -Lstandalone_devices/thorlabs_powermeter/dll \
    -lTLPM_64 \
    -lfltk_images -lpng -lz -lfltk -lgdi32 -lcomdlg32 -lcomctl32 -lole32 -luuid -lws2_32 -lkernel32 -luser32 -lgdiplus -lshell32 -lwinspool -ladvapi32 -static-libgcc

if [ -f build/windows/Template.exe ]; then
    echo "Build successful! Executable: build/windows/Template.exe"
    
    echo "Copying required DLLs..."
    cp standalone_devices/thorlabs_powermeter/dll/TLPM_64.dll build/windows/ || true
    
    if [ "$START_AFTER_BUILD" = true ]; then
        echo "Starting application..."
        ./build/windows/Template.exe &
    fi
else
    echo "Build failed. Check error messages above."
    exit 1
fi
