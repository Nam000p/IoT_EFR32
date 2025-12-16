#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h> 

// SERIAL PORT CONFIGURATION
// Change "COM10" to match your actual port number (e.g., COM3, COM6)
#define COM_PORT_NAME "\\\\.\\COM10" 
#define BAUD_RATE CBR_115200 
#define MAX_BUFFER_SIZE 256

// $ gcc serial_app.c -o serial_app.exe
// $ ./serial_app.exe

/* Function Prototypes */
HANDLE open_serial_port();
void configure_port(HANDLE hSerial);
void process_raw_data(const char* raw_string);
void save_to_file(float temp, float hum);
void serial_loop(HANDLE hSerial);

int main() {
    /* 1. Open the Serial Port */
    HANDLE hSerial = open_serial_port();
    if (hSerial == INVALID_HANDLE_VALUE) return 1;
    
    /* 2. Configure UART settings (Baudrate, Parity, etc.) */
    configure_port(hSerial);
    
    /* 3. Display User Interface */
    printf("\n--- EFR32 Data Receiver App (CSV Format) ---\n");
    printf("Expected format: 'Humidity;Temperature' (e.g., 61;26.76)\n");
    printf("Data will be appended to 'sensor_data.csv'.\n");
    printf("Press 'q' to quit.\n\n");

    /* 4. Start the main listening loop */
    serial_loop(hSerial);

    /* 5. Cleanup and Exit */
    CloseHandle(hSerial);
    return 0;
}

// --- DATA PROCESSING FUNCTION (Replaces JSON parsing) ---
void process_raw_data(const char* s) {
    float temp = 0.0;
    float hum = 0.0;
    
    // Sample Data: "61;26.76"
    // "%f;%f" means: Find a float, then a semicolon, then another float.
    // 'count' stores the number of variables successfully read.
    int count = sscanf(s, "%f;%f", &hum, &temp);

    // Only proceed if exactly 2 values were found
    if (count == 2) {
        printf("[DATA] Temp: %.2f C, Hum: %.2f %%\n", temp, hum);
        save_to_file(temp, hum);
    } 
    // Lines starting with debug info (like "RH=...") will fail sscanf (count != 2)
    // and are automatically ignored/skipped here.
    else {
        // Uncomment the line below to see skipped lines for debugging
        // printf("[SKIP] %s\n", s);
    }
}

// --- CSV SAVING FUNCTION ---
void save_to_file(float temp, float hum) {
    /* Open file in Append mode */
    FILE *f = fopen("sensor_data.csv", "a");
    if (f == NULL) return;

    /* Get current timestamp */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    /* Write: Timestamp, Temperature, Humidity */
    fprintf(f, "%s,%.2f,%.2f\n", time_str, temp, hum);
    printf("[SAVED] Saved to CSV.\n");
    
    fclose(f);
}

// --- LOW-LEVEL SERIAL HELPER FUNCTIONS ---

/* Open the COM port file handle */
HANDLE open_serial_port() {
    HANDLE hSerial = CreateFileA(COM_PORT_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) fprintf(stderr, "ERROR: Could not open COM port.\n");
    return hSerial;
}

/* Configure BaudRate, ByteSize, StopBits, Parity */
void configure_port(HANDLE hSerial) {
    DCB dcb = {0}; 
    dcb.DCBlength = sizeof(dcb); 
    GetCommState(hSerial, &dcb);
    
    dcb.BaudRate = BAUD_RATE; 
    dcb.ByteSize = 8; 
    dcb.StopBits = ONESTOPBIT; 
    dcb.Parity = NOPARITY;
    
    SetCommState(hSerial, &dcb);
    
    /* Set Timeouts to prevent blocking indefinitely */
    COMMTIMEOUTS to = {0}; 
    to.ReadIntervalTimeout = 50; 
    to.ReadTotalTimeoutConstant = 50; 
    to.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &to);
    
    printf("COM port connected successfully.\n");
}

/* Main Loop: Read characters one by one to form lines */
void serial_loop(HANDLE hSerial) {
    char c, buf[MAX_BUFFER_SIZE]; 
    DWORD r; 
    int idx = 0;
    
    while (1) {
        /* Check if user pressed 'q' to quit */
        if (_kbhit()) { if (_getch() == 'q') break; }
        
        /* Read 1 byte from serial */
        if (ReadFile(hSerial, &c, 1, &r, NULL) && r == 1) {
            
            /* If Newline (\n) is detected, process the complete buffer */
            if (c == '\n') {
                buf[idx] = 0; // Null-terminate string
                if (idx > 0) process_raw_data(buf); // Process the line
                idx = 0; // Reset buffer index
            } 
            /* Ignore Carriage Return (\r) */
            else if (idx < MAX_BUFFER_SIZE - 1 && c != '\r') {
                buf[idx++] = c;
            }
        }
        /* Small sleep to reduce CPU usage */
        Sleep(1);
    }
}