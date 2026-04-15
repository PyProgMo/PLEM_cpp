#!/bin/bash
# Build script for Windows (MSYS2 MinGW 64-bit)
# Requires FLTK installed via MSYS2: pacman -S mingw-w64-x86_64-fltk

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
$COMPILER -std=c++17 -Iinclude -I/mingw64/include src/main.cpp -o build/windows/Template.exe -mwindows -L/mingw64/lib -lfltk -lgdi32 -lcomdlg32 -lcomctl32 -lole32 -luuid -lws2_32 -lkernel32 -luser32 -lgdiplus -lshell32 -lwinspool -ladvapi32 -static-libgcc

if [ -f build/windows/Template.exe ]; then
    echo "Build successful! Executable: build/windows/Template.exe"
else
    echo "Build failed. Check error messages above."
    exit 1
fi
