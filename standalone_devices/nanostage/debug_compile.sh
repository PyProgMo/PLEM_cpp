#!/bin/bash
cd /c/Users/PLM_104/Desktop/PLEMv5/standalone_devices/nanostage
echo "Current directory: $(pwd)"
echo "Files:"
ls -la *.c
echo "===== Attempting gcc compile ====="
gcc -c test_hello.c -o test_hello.o
echo "Exit code: $?"
echo "===== Checking for object file ====="
ls -la test_hello.o || echo "File not found"
echo "===== Done ====="
