#!/bin/bash
# Build script for Linux
# Requires FLTK installed and g++ in PATH

g++ -std=c++17 -Iinclude -I/usr/include src/main.cpp -o build/linux/screener -L/usr/lib -lfltk -static -static-libgcc -static-libstdc++
