#include <stdio.h>
#include <windows.h>

int main() {
    printf("Testing DLL loading...\n");
    fflush(stdout);
    
    HMODULE hModule = LoadLibraryA("E816_DLL.dll");
    if (hModule == NULL) {
        printf("ERROR: Failed to load E816_DLL.dll\n");
        printf("Error code: %ld\n", GetLastError());
        return 1;
    }
    
    printf("OK: E816_DLL.dll loaded successfully\n");
    
    // Try to get a function address
    FARPROC proc = GetProcAddress(hModule, "E816_ConnectUSB");
    if (proc == NULL) {
        printf("ERROR: E816_ConnectUSB not found in DLL\n");
        printf("Error code: %ld\n", GetLastError());
        return 1;
    }
    
    printf("OK: E816_ConnectUSB found at %p\n", proc);
    
    FreeLibrary(hModule);
    printf("DLL unloaded successfully\n");
    
    return 0;
}
