#!/bin/bash
# Build script for macOS
# Requires FLTK installed and g++ in PATH

g++ -std=c++17 -Iinclude -I/usr/local/include src/main.cpp -o build/mac/screener -L/usr/local/lib -lfltk -static-libgcc -static-libstdc++
