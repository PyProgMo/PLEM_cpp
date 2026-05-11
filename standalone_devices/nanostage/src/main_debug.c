#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "E816_DLL.h"
#include "picontrollererrors.h"

int main(int argc, char* argv[])
{
    int deviceID = -1;
    char buffer[256];
    int errorCode;
    
    // Open log file
    FILE* logfile = fopen("test_output.log", "w");
    if (!logfile) {
        printf("ERROR: Cannot open log file\n");
        return 1;
    }
    
    fprintf(logfile, "========================================\n");
    fprintf(logfile, "E816 Nanoscale Stage Controller - Debug Test\n");
    fprintf(logfile, "========================================\n\n");
    fprintf(logfile, "Starting tests...\n");
    fflush(logfile);

    // Attempt to connect via USB
    fprintf(logfile, "[1] Attempting USB connection...\n");
    fflush(logfile);
    deviceID = E816_ConnectUSB("109021162");
    fprintf(logfile, "    Result: %d\n", deviceID);
    fflush(logfile);
    
    if (deviceID < 0) {
        fprintf(logfile, "    USB connection failed (ID: %d)\n", deviceID);
        fprintf(logfile, "    Trying RS232 on COM3...\n");
        fflush(logfile);
        
        deviceID = E816_ConnectRS232(3, 57600);
        fprintf(logfile, "    RS232 Result: %d\n", deviceID);
        fflush(logfile);
        
        if (deviceID < 0) {
            fprintf(logfile, "    [ERROR] Could not establish connection to device.\n");
            fflush(logfile);
            fclose(logfile);
            return 1;
        }
    }
    
    fprintf(logfile, "    Connection successful! Device ID: %d\n\n", deviceID);
    fflush(logfile);

    // Verify connection
    fprintf(logfile, "[2] Verifying connection...\n");
    BOOL connected = E816_IsConnected(deviceID);
    fprintf(logfile, "    IsConnected result: %d\n", connected);
    fflush(logfile);
    
    if (!connected) {
        fprintf(logfile, "    [ERROR] Device is not responding.\n");
        fflush(logfile);
        fclose(logfile);
        return 1;
    }
    fprintf(logfile, "    Device is connected and responsive.\n\n");
    fflush(logfile);

    // Query device ID
    fprintf(logfile, "[3] Querying device identification...\n");
    memset(buffer, 0, sizeof(buffer));
    BOOL qResult = E816_qIDN(deviceID, buffer, sizeof(buffer) - 1);
    fprintf(logfile, "    qIDN result: %d, buffer: '%s'\n", qResult, buffer);
    fflush(logfile);
    
    if (!qResult) {
        fprintf(logfile, "    [ERROR] Could not query device ID.\n");
        errorCode = E816_GetError(deviceID);
        fprintf(logfile, "    Error code: %d\n", errorCode);
        fflush(logfile);
    } else {
        fprintf(logfile, "    Device ID: %s\n", buffer);
        fflush(logfile);
    }

    // Get error status
    fprintf(logfile, "\n[4] Getting error status...\n");
    int err = 0;
    BOOL errResult = E816_qERR(deviceID, &err);
    fprintf(logfile, "    Error result: %d, error code: %d\n", errResult, err);
    fflush(logfile);

    // Close connection
    fprintf(logfile, "\n[5] Closing connection...\n");
    E816_CloseConnection(deviceID);
    fprintf(logfile, "    Connection closed.\n");
    fflush(logfile);

    fprintf(logfile, "\n========================================\n");
    fprintf(logfile, "Test completed successfully!\n");
    fprintf(logfile, "========================================\n");
    fflush(logfile);
    fclose(logfile);
    
    printf("Test log written to test_output.log\n");
    return 0;
}
