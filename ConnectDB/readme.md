# Windows Serial Data Receiver (CSV)

A lightweight C application designed to read sensor data (Temperature and Humidity) from an embedded system (like EFR32, Arduino, or ESP32) via a Serial/COM port. The data is displayed in real-time and automatically saved to a CSV file.

## 📋 Features

* **Real-time Monitoring:** Reads serial data streams continuously.
* **CSV Parsing:** Designed to parse simple format `Humidity;Temperature` (e.g., `61;26.76`).
* **Data Logging:** Automatically appends valid data to `sensor_data.csv` with a timestamp.
* **Robustness:** Ignores debug logs or malformed lines automatically.
* **No External Dependencies:** Uses standard Windows API (`windows.h`), making it easy to compile.

## 🛠️ Prerequisites

* **Operating System:** Windows 10/11.
* **Compiler:** GCC (via MinGW) or Visual Studio (MSVC).
* **Hardware:** An embedded board (e.g., EFR32BG22) transmitting data via UART/USB.

## ⚙️ Configuration

Before compiling, open `serial_app.c` and verify the settings at the top of the file:

```c
// Change COM10 to the port your device is connected to (Check Device Manager)
#define COM_PORT_NAME "\\\\.\\COM10" 

// Match the Baud Rate of your embedded device
#define BAUD_RATE CBR_115200
```
Note: The format \\\\.\\COM10 is required for ports COM10 and above. For COM1-COM9, you can simply use "COM3".

## 🚀 Compilation
### Using GCC (MinGW) - Recommended
Open a terminal in the project folder and run:
```c
gcc serial_app.c -o serial_app.exe
```
## 🏃 Usage

1. Connect your embedded device to the PC.
2. Ensure no other software (Putty, Simplicity Studio) is using the COM port.
3. Run the application:
```c
./serial_app.exe
```
4. **To Quit:** Press q on your keyboard.

## 📊 Data Format
### Input (From Microcontroller)
The software expects a string ending with a newline \n. Format: Humidity;Temperature Example:
```c
61;26.76
```
### Output (CSV File)
The sensor_data.csv file will be generated automatically:
```c
2025-12-16 16:30:01,26.76,61.00
2025-12-16 16:30:02,26.80,61.50
```

## 🐛 Troubleshooting
* **"Error: Could not open COM port":** * Check if the port number in the code matches Device Manager.
    * Close other serial terminals (Putty, RealTerm, etc.).
* **No Data Appearing:**
    * Check if TX/RX wires are connected correctly.
    * Verify the Baud Rate (115200) matches the microcontroller code.

## 📝 License
This project is open-source. Feel free to modify and distribute.