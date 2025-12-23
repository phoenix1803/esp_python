# ESP Water Quality Sensor Data Logger

An open-source Python application for collecting and logging real-time water quality sensor data from ESP32-based multi-electrode sensor systems via serial communication.

## Overview

This application interfaces with an ESP32 microcontroller that monitors multiple water quality parameters including UV absorbance, TDS (Total Dissolved Solids), pH, and heavy metal concentrations using electrochemical sensors. The system operates in two modes: live monitoring and batch data collection.

## Features

- **Live Monitoring Mode**: Real-time display of incoming sensor data from ESP32
- **Batch Capture Mode**: Automated collection of 7 readings with 10-second sampling windows
- **Data Averaging**: Automatically averages multiple samples per reading for improved accuracy
- **JSON Export**: Saves averaged readings as structured JSON files
- **Non-blocking Keyboard Input**: Switch between modes without interrupting serial communication
- **Error Handling**: Robust error handling for corrupted or invalid serial data

## Sensor Parameters

The application collects and logs the following parameters:

- **UV** - UV absorbance (4 decimal places)
- **TDS** - Total Dissolved Solids in ppm (4 decimal places)
- **pH** - pH level (4 decimal places)
- **Pt** - Platinum electrode reading (3 decimal places)
- **Cu** - Copper electrode reading (3 decimal places)
- **Zn** - Zinc electrode reading (3 decimal places)
- **SS** - Stainless Steel electrode reading (3 decimal places)
- **Ag** - Silver electrode reading (3 decimal places)
- **Graphite** - Graphite electrode reading (3 decimal places)
- **Temp** - Temperature in °C (2 decimal places)

## Requirements

- Python 3.x
- `pyserial` library

Install dependencies:
```bash
pip install pyserial
```

## Configuration

Edit the following constants in `esp.py` to match your setup:

```python
PORT = "COM5"           # Serial port (COM5 on Windows, /dev/ttyUSB0 on Linux)
BAUD = 115200           # Baud rate (must match ESP32 configuration)
SAVE_DIR = "esp_readings"  # Directory for saved readings
TOTAL_READINGS = 7      # Number of readings to collect
WINDOW_TIME = 10        # Seconds per reading window
```

## Usage

1. **Connect your ESP32** to the computer via USB
2. **Verify the COM port** in Device Manager (Windows) or `ls /dev/tty*` (Linux)
3. **Run the application**:
   ```bash
   python esp.py
   ```
4. **Live Mode**: Observe real-time sensor data streaming from ESP32
5. **Press 'c'** to start batch capture mode
6. **Wait for collection**: The application will automatically collect 7 readings, each averaged over 10 seconds
7. **Find your data**: JSON files will be saved in the `esp_readings/` directory as `reading_1.json`, `reading_2.json`, etc.

## Output Format

Each saved reading is a JSON file with averaged sensor values:

```json
{
    "UV": 0.0009,
    "TDS": 1.8475,
    "pH": 0.1303,
    "Pt": 38.376,
    "Cu": 31.408,
    "Zn": 9.756,
    "SS": 37.6,
    "Ag": 10.089,
    "Graphite": 37.577,
    "Temp": 25.3
}
```

## How It Works

### Live Mode
- Continuously reads JSON data from the serial port
- Displays incoming sensor readings in real-time
- Non-blocking keyboard monitoring waits for 'c' key press

### Batch Capture Mode
- Collects samples for 10 seconds per reading
- Stores all valid JSON samples in memory
- Calculates the average of all samples for each parameter
- Rounds values to appropriate precision
- Saves as formatted JSON file
- Repeats for 7 readings total

### Data Averaging Algorithm
The `average_samples()` function computes the arithmetic mean for each sensor parameter across all collected samples within the time window, reducing noise and improving measurement accuracy.

## Known Issues

- **Temperature Sensor**: May display -127.0°C if DS18B20 sensor is disconnected or faulty
- **Negative UV Values**: Can occur due to electrochemical interference when reactive metal electrodes (especially Zinc) are present in the solution, caused by galvanic cell formation with the reference electrode

## Platform Notes

- **Windows**: Uses `msvcrt` for non-blocking keyboard input
- **Linux/Mac**: Would require modification to use `termios` or `select` for keyboard input

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve functionality, add features, or fix bugs.

## Hardware Requirements

- ESP32 microcontroller
- Multi-electrode sensor array (Pt, Cu, Zn, SS, Ag, Graphite)
- UV absorbance sensor
- TDS sensor
- pH sensor
- DS18B20 temperature sensor
- Ag/AgCl reference electrode (recommended)

## Author

Created for water quality monitoring and electrochemical sensor research applications.
