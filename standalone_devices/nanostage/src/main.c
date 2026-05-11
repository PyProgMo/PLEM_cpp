#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "E816_DLL.h"
#include "picontrollererrors.h"

static void print_connection_status(int deviceID)
{
    if (deviceID >= 0) {
        printf("[STATUS] Connected (Device ID: %d)\n", deviceID);
    } else {
        printf("[STATUS] Not connected\n");
    }
}

static void print_error_text(int errorCode)
{
    char buffer[256];

    memset(buffer, 0, sizeof(buffer));
    if (E816_TranslateError(errorCode, buffer, sizeof(buffer) - 1)) {
        printf("    Error description: %s\n", buffer);
    }
}

static int try_preset_usb_connection(const char* usbDescription)
{
    int deviceID;

    if (usbDescription == NULL || usbDescription[0] == '\0') {
        printf("[ERROR] Empty USB description/ID provided.\n");
        return 2;
    }

    printf("[AUTO] Trying USB connection with preset ID: %s\n", usbDescription);
    deviceID = E816_ConnectUSB(usbDescription);
    if (deviceID < 0) {
        printf("[AUTO] USB connection failed (ID: %d)\n", deviceID);
        return 1;
    }

    printf("[AUTO] USB connection successful. Device ID: %d\n", deviceID);
    E816_CloseConnection(deviceID);
    printf("[AUTO] Connection closed.\n");
    return 0;
}

int main(int argc, char* argv[])
{
    int deviceID = -1;
    char buffer[256];
    int errorCode;
    char input[128];
    int running = 1;

    /* Non-interactive mode for build scripts: test one preset USB ID and exit. */
    if (argc == 3 && strcmp(argv[1], "--connect-usb-id") == 0) {
        return try_preset_usb_connection(argv[2]);
    }
    
    printf("========================================\n");
    printf("E816 Nanoscale Stage Controller - Test\n");
    printf("========================================\n\n");
    fflush(stdout);

    while (running) {
        printf("\n----------------------------------------\n");
        print_connection_status(deviceID);
        printf("----------------------------------------\n");
        printf("1) Connect USB\n");
        printf("2) Connect RS232\n");
        printf("3) Query device ID\n");
        printf("4) Query device error\n");
        printf("5) Move relative\n");
        printf("6) Close connection\n");
        printf("7) Exit\n");
        printf("> ");

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        switch (atoi(input)) {
            case 1:
                if (deviceID >= 0) {
                    E816_CloseConnection(deviceID);
                    deviceID = -1;
                }

                printf("Connecting via USB...\n");
                deviceID = E816_ConnectUSB("109021162");
                if (deviceID < 0) {
                    printf("    USB connection failed (ID: %d)\n", deviceID);
                } else {
                    printf("    USB connection successful. Device ID: %d\n", deviceID);
                }
                break;

            case 2:
            {
                int portNr = 3;
                int baudRate = 57600;

                if (deviceID >= 0) {
                    E816_CloseConnection(deviceID);
                    deviceID = -1;
                }

                printf("Connecting via RS232 (COM%d, %d baud)...\n", portNr, baudRate);
                deviceID = E816_ConnectRS232(portNr, baudRate);
                if (deviceID < 0) {
                    printf("    RS232 connection failed (ID: %d)\n", deviceID);
                } else {
                    printf("    RS232 connection successful. Device ID: %d\n", deviceID);
                }
                break;
            }

            case 3:
                if (deviceID < 0) {
                    printf("    No active connection.\n");
                    break;
                }

                memset(buffer, 0, sizeof(buffer));
                if (!E816_qIDN(deviceID, buffer, sizeof(buffer) - 1)) {
                    printf("    [ERROR] Could not query device ID.\n");
                    errorCode = E816_GetError(deviceID);
                    printf("    Error code: %d\n", errorCode);
                    print_error_text(errorCode);
                } else {
                    printf("    Device ID: %s\n", buffer);
                }
                break;

            case 4:
                if (deviceID < 0) {
                    printf("    No active connection.\n");
                    break;
                }

                if (E816_qERR(deviceID, &errorCode)) {
                    if (errorCode == 0) {
                        printf("    No errors detected.\n");
                    } else {
                        printf("    Error detected: %d\n", errorCode);
                        print_error_text(errorCode);
                    }
                } else {
                    printf("    Could not query error status.\n");
                }
                break;

            case 5:
            {
                char axisName[32];
                double delta = 0.0;

                if (deviceID < 0) {
                    printf("    No active connection.\n");
                    break;
                }

                printf("    Axis name (for example X, Y, Z): ");
                if (!fgets(axisName, sizeof(axisName), stdin)) {
                    break;
                }
                axisName[strcspn(axisName, "\r\n")] = '\0';

                printf("    Relative move distance: ");
                if (!fgets(input, sizeof(input), stdin)) {
                    break;
                }
                delta = atof(input);

                if (!E816_MVR(deviceID, axisName, &delta)) {
                    printf("    [ERROR] Move command failed.\n");
                    errorCode = E816_GetError(deviceID);
                    printf("    Error code: %d\n", errorCode);
                    print_error_text(errorCode);
                } else {
                    printf("    Move command sent.\n");
                }
                break;
            }

            case 6:
                if (deviceID >= 0) {
                    E816_CloseConnection(deviceID);
                    deviceID = -1;
                }
                printf("    Connection closed.\n");
                break;

            case 7:
                running = 0;
                break;

            default:
                printf("    Unknown option.\n");
                break;
        }
    }

    if (deviceID >= 0) {
        E816_CloseConnection(deviceID);
    }

    printf("\nExiting.\n");
    return 0;
}
