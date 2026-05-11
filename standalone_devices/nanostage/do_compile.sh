#!/bin/bash
# Direct compilation test
cd /c/Users/PLM_104/Desktop/PLEMv5/standalone_devices/nanostage

# Convert line endings CRLF to LF
for file in src/main.c include/E816_DLL.h E816_DLL/E816_DLL.h; do
  if [ -f "$file" ]; then
    dos2unix "$file" 2>/dev/null || sed -i 's/\r$//' "$file"
  fi
done

# Now try compilation
gcc -Wall -O2 -std=c99 -Iinclude -IE816_DLL -c src/main.c -o build/obj/main.o
if [ $? -eq 0 ]; then
  echo "SUCCESS: Compiled main.c"
  ls -lh build/obj/main.o
else
  echo "FAILED: Compilation error"
fi
