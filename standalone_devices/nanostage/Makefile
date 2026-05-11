# Makefile for E816 NanoStage Controller - MinGW32 (MSYS2)
# ========================================================

# Compiler and tools
CC = /mingw32/bin/gcc -B/mingw32/bin
LD = /mingw32/bin/gcc -B/mingw32/bin
MSYS2_MINGW32_BIN = /mingw32/bin
DLLTOOL = $(MSYS2_MINGW32_BIN)/dlltool
GENDEF = $(MSYS2_MINGW32_BIN)/gendef

# Project paths
SRC_DIR = src
INCLUDE_DIR = include
E816_DLL_DIR = E816_DLL
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source and object files
SOURCES = $(SRC_DIR)/main.c
OBJECTS = $(OBJ_DIR)/main.o
EXECUTABLE = $(BUILD_DIR)/test.exe

# DLL and import library files
DLL_FILE = $(E816_DLL_DIR)/E816_DLL.dll
DLL_LIB = $(E816_DLL_DIR)/libPI_E816.a
DLL_DEF = $(E816_DLL_DIR)/E816_DLL.def

# Compiler flags
CFLAGS = -Wall -O2 -std=c99
INCLUDE_FLAGS = -I$(INCLUDE_DIR) -I$(E816_DLL_DIR)
LDFLAGS = -L$(E816_DLL_DIR) -lPI_E816

# Phony targets
.PHONY: all clean build-dirs help check-dll generate-lib run

# Default target
all: check-dll build-dirs $(EXECUTABLE)

# Help target
help:
	@echo "E816 NanoStage Controller - MinGW32 Project"
	@echo "=========================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make build         - Build the project"
	@echo "  make run           - Build and run the executable"
	@echo "  make generate-lib  - Generate import library from DLL"
	@echo "  make clean         - Remove build artifacts"
	@echo "  make help          - Show this help message"
	@echo ""
	@echo "Notes:"
	@echo "  - Run this from the MSYS2 MinGW 32-bit shell"
	@echo "  - Ensure E816_DLL.dll is present in ./E816_DLL/"
	@echo "  - The import library will be generated if not found"
	@echo ""

# Check if DLL exists
check-dll:
	@if [ ! -f "$(DLL_FILE)" ]; then \
		echo "ERROR: $(DLL_FILE) not found!"; \
		echo "Make sure the DLL is in the E816_DLL directory."; \
		exit 1; \
	fi
	@echo "[OK] DLL found: $(DLL_FILE)"

# Create build directories
build-dirs:
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir -p "$(OBJ_DIR)"; fi

# Generate import library from DLL (if needed)
generate-lib: $(DLL_FILE)
	@echo "[*] Generating import library from $(DLL_FILE)..."
	@echo "    Step 1: Running gendef..."; \
	cd $(E816_DLL_DIR) && rm -f E816_DLL.def E816_DLL.raw.def libPI_E816.a && $(GENDEF) ./E816_DLL.dll; \
	if [ ! -s "E816_DLL.def" ]; then \
		echo "ERROR: Failed to generate E816_DLL.def"; \
		exit 1; \
	fi; \
	cp E816_DLL.def E816_DLL.raw.def; \
	echo "    Step 2: Normalizing export names (remove @N suffixes)..."; \
	sed -E 's/@[0-9]+$$//' E816_DLL.raw.def > E816_DLL.def; \
	echo "    Step 3: Running dlltool..."; \
	$(DLLTOOL) -d E816_DLL.def -l libPI_E816.a && cd ..; \
	if [ -f "$(DLL_LIB)" ]; then \
		echo "[OK] Import library generated: $(DLL_LIB)"; \
	else \
		echo "ERROR: Failed to generate $(DLL_LIB)"; \
		exit 1; \
	fi

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[CC] $< -> $@"
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Link the executable
$(EXECUTABLE): generate-lib $(OBJECTS)
	@echo "[LD] Linking $(EXECUTABLE)..."
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "[OK] Executable created: $(EXECUTABLE)"
	@echo ""
	@echo "[*] Copying DLL to $(BUILD_DIR)..."
	@cp $(DLL_FILE) $(BUILD_DIR)/
	@echo "[OK] DLL copied"

# Build target (alias for all)
build: all

# Run the executable
run: all
	@echo "[RUN] Executing $(EXECUTABLE)..."
	@echo ""
	@cd $(BUILD_DIR) && ./test.exe

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing build artifacts..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		rm -rf "$(BUILD_DIR)"; \
	fi
	@echo "[OK] Clean complete"

# Information target
info:
	@echo "Project Configuration"
	@echo "====================="
	@echo "Source directory:  $(SRC_DIR)"
	@echo "Include directory: $(INCLUDE_DIR)"
	@echo "DLL directory:     $(E816_DLL_DIR)"
	@echo "Build directory:   $(BUILD_DIR)"
	@echo "DLL file:          $(DLL_FILE)"
	@echo "Import library:    $(DLL_LIB)"
	@echo "Executable:        $(EXECUTABLE)"
	@echo ""
	@echo "Compilation flags: $(CFLAGS)"
	@echo "Include flags:     $(INCLUDE_FLAGS)"
	@echo "Linker flags:      $(LDFLAGS)"

.PHONY: info
