# ESP32 Water Quality Sensor Data Logger

An open-source Python application for collecting and logging real-time water quality sensor data from ESP32-based multi-electrode sensor systems via serial communication.

## Overview

This project consists of two main components:
1. **ESP32 Firmware** (`arduino/sketch/test.ino`) - Reads sensor data and streams JSON over serial
2. **Python Data Logger** (`esp.py`) - Collects, averages, and saves sensor readings

The system monitors multiple water quality parameters including UV absorbance, TDS (Total Dissolved Solids), pH, and heavy metal concentrations using electrochemical sensors with multiple ADCs.

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

## Python Application Configuration

Edit the following constants in `esp.py` to match your setup:

```python
PORT = "COM5"           # Serial port (COM5 on Windows, /dev/ttyUSB0 on Linux)
BAUD = 115200           # Baud rate (must match ESP32 configuration)
SAVE_DIR = "esp_readings"  # Directory for saved readings
TOTAL_READINGS = 7      # Number of readings to collect
WINDOW_TIME = 10        # Seconds per reading window
```

## Usage (Python Application)

1. **Connect your ESP32** to the computer via USB
2. **Verify the COM port** in Device Manager (Windows) or `ls /dev/tty*` (Linux)
3. **Upload the ESP32 firmware** (see Setup Instructions above)
4. **Run the Python application**:
   ```bash
   python esp.py
   ```
5. **Live Mode**: Observe real-time sensor data streaming from ESP32
6. **Press 'c'** to start batch capture mode
7. **Wait for collection**: The application will automatically collect 7 readings, each averaged over 10 seconds
8. **Find your data**: JSON files will be saved in the `esp_readings/` directory as `reading_1.json`, `reading_2.json`, etc.

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

### Temperature Sensor
- May display **-127.0°C** if DS18B20 sensor is:
  - Not connected properly
  - Using wrong GPIO pin
  - Missing pull-up resistor (4.7kΩ required)
  - Faulty or damaged
  
**Fix**: Check wiring, verify 4.7kΩ pull-up resistor between Data and VCC, test with a known-good DS18B20

### Negative UV Values
Can occur due to **electrochemical interference** when reactive metal electrodes (especially Zinc) are present in the solution. This is caused by:
- **Galvanic cell formation** between Zn (-0.76V) and Ag/AgCl reference (+0.22V)
- Current flow through solution creates voltage shift
- UV sensor reads relative to shifted reference → negative value

**Why Zn specifically?**
- Zinc is highly reactive (large voltage difference from reference)
- Platinum and Graphite are inert (no significant galvanic effect)

**Workarounds:**
- Use high-impedance buffer on reference electrode
- Implement three-electrode setup (Working/Reference/Counter)
- Add series resistance to Zn electrode
- Measure UV before inserting Zn electrode
- Clean/replace corroded electrodes
- Use fresh reference electrode if aged

### ADC Crosstalk
- With four ADC modules, there may be slight crosstalk between channels
- **Mitigation**: Use differential mode for electrodes (already implemented)

## Troubleshooting

### ESP32 Not Detected
- Install CP210x or CH340 USB drivers for your ESP32 board
- Try a different USB cable (some are power-only)
- Check Device Manager (Windows) or `dmesg` (Linux) for connection

### No Serial Data
- Verify baud rate is 115200 in both ESP32 code and Python script
- Check Serial Monitor first to confirm ESP32 is transmitting
- Ensure only one application accesses the serial port at a time

### ADC Not Found Error
- Verify I2C wiring (SDA to GPIO 21, SCL to GPIO 22)
- Check I2C addresses are correct (0x48, 0x49, 0x4A, 0x4B)
- Run I2C scanner sketch to detect connected devices
- Ensure ADC modules are powered (3.3V or 5V)

### Erratic Sensor Readings
- Check all ground connections
- Add 100nF capacitors between sensor inputs and ground
- Verify power supply is stable (5V/3.3V)
- Keep sensor wires away from power lines (noise coupling)

### Python Script Crashes
- Ensure `pyserial` is installed: `pip install pyserial`
- Check COM port is correct
- Verify ESP32 is sending valid JSON (check via Serial Monitor)

## Project Structure

```
esp_readings/
├── arduino/
│   └── sketch/
│       └── test.ino          # ESP32 firmware
├── esp_readings/              # Output directory for JSON files
│   ├── reading_1.json
│   ├── reading_2.json
│   └── ...
├── esp.py                     # Python data logger
├── main.py                    # (Optional) Alternative script
└── README.md                  # This file
```

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve functionality, add features, or fix bugs.

## Hardware Requirements

### ESP32 Setup
- **ESP32 Development Board** (any variant with I2C support)
- **Four ADS1115 16-bit ADC modules** (I2C addresses: 0x48, 0x49, 0x4A, 0x4B)
- **DS18B20 Temperature Sensor** (1-Wire)
- **Sensors:**
  - UV absorbance sensor
  - TDS (Total Dissolved Solids) sensor
  - pH sensor
  - Metal electrodes: Platinum (Pt), Copper (Cu), Zinc (Zn), Stainless Steel (SS), Silver (Ag), Graphite
  - Ag/AgCl reference electrode (recommended for stable measurements)

### Wiring Connections

#### I2C Bus (Shared)
- **SDA**: GPIO 21
- **SCL**: GPIO 22
- Connect all four ADS1115 modules to the same I2C bus with different addresses

#### ADS1115 Module Assignments
- **0x48** (adsSensors): UV (A0), TDS (A1), pH (A2)
- **0x49** (adsE1): Pt (differential A0-A1), Cu (differential A2-A3)
- **0x4A** (adsE2): Zn (differential A0-A1), SS (differential A2-A3)
- **0x4B** (adsE3): Ag (differential A0-A1), Graphite (differential A2-A3)

#### Temperature Sensor
- **Data**: GPIO 15
- **VCC**: 3.3V
- **GND**: Ground
- Add 4.7kΩ pull-up resistor between Data and VCC

#### Power
- **ESP32 VCC**: 5V via USB or external power
- **ADC Modules**: 3.3V or 5V (check module specifications)
- **Common Ground**: Connect all grounds together

## Software Requirements

### ESP32 Firmware
- **Arduino IDE** or **PlatformIO**
- **ESP32 Board Support**: Install via Arduino Board Manager
- **Required Libraries:**
  - `Adafruit_ADS1X15` (for ADC modules)
  - `OneWire` (for DS18B20)
  - `DallasTemperature` (for DS18B20)
  - `WiFi` (built-in, optional for future expansion)

### Python Application
- **Python 3.x**
- **pyserial** library

Install Python dependencies:
```bash
pip install pyserial
```

## Setup Instructions

### Step 1: Hardware Assembly
1. Connect all four ADS1115 modules to ESP32 I2C bus (GPIO 21/22)
2. Configure I2C addresses on each ADS1115 (using ADDR pin)
3. Wire sensors to appropriate ADC channels (see wiring diagram above)
4. Connect DS18B20 temperature sensor to GPIO 15 with pull-up resistor
5. Ensure all components share common ground

### Step 2: Upload ESP32 Firmware
1. Open Arduino IDE
2. Install ESP32 board support:
   - Go to **File → Preferences**
   - Add to Additional Board Manager URLs: 
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search for "ESP32" and install
3. Install required libraries:
   - Go to **Sketch → Include Library → Manage Libraries**
   - Install: `Adafruit ADS1X15`, `OneWire`, `DallasTemperature`
4. Open `arduino/sketch/test.ino`
5. Select your ESP32 board under **Tools → Board**
6. Select the correct COM port under **Tools → Port**
7. Click **Upload** (or press Ctrl+U)
8. Open Serial Monitor (115200 baud) to verify JSON data is streaming

### Step 3: Configure Python Application
1. Identify your ESP32's COM port:
   - **Windows**: Check Device Manager → Ports (COM & LPT)
   - **Linux**: Run `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`
   - **Mac**: Run `ls /dev/tty.*`
2. Edit `esp.py` and update the port:
   ```python
   PORT = "COM5"  # Change to your port
   ```

### Step 4: Run the Application
1. Ensure ESP32 is connected and streaming data (check Serial Monitor)
2. Close Arduino Serial Monitor (can't share port)
3. Run the Python script:
   ```bash
   python esp.py
   ```
4. You'll see live sensor data streaming
5. Press **'c'** to start batch capture mode
6. Wait for 7 readings to be collected (10 seconds each)
7. JSON files will be saved in `esp_readings/` directory

## ESP32 Firmware Details

### How It Works
- Reads all sensors every 1 second
- Streams data as JSON over serial at 115200 baud
- Uses differential mode for electrode measurements (reduces noise)
- Applies appropriate LSB scaling factors for voltage conversion

### Key Configuration
```cpp
// ADC Gain Setting (±4.096V range)
GAIN_ONE

// LSB (Least Significant Bit) values
LSB_sensors = 0.0001875  // For UV, TDS, pH
LSB_electrodes = 0.0078125  // For metal electrodes

// Sample Rate
1 second per reading
```

### Modifying WiFi (Optional)
The sketch includes WiFi capability (currently unused). To enable:
```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

## Platform Notes

- **Windows**: Uses `msvcrt` for non-blocking keyboard input ✓
- **Linux/Mac**: Would require modification to use `termios` or `select` for keyboard input

## Applications

This system is designed for:
- **Water quality monitoring** in lakes, rivers, and treatment plants
- **Heavy metal detection** in industrial wastewater
- **Environmental research** and data collection
- **Educational projects** on electrochemistry and sensors
- **Laboratory experiments** requiring multi-parameter logging

## License

This project is open source and available under the **MIT License**.

## Contributing

Contributions are welcome! Feel free to:
- Submit issues for bugs or feature requests
- Create pull requests with improvements
- Share your sensor configurations and results
- Improve documentation

## Citation

If you use this project in your research or publication, please cite:
```
ESP32 Water Quality Sensor Data Logger
GitHub: https://github.com/phoenix1803/esp_python
```

## Support

For questions, issues, or suggestions:
- Open an issue on GitHub
- Check existing issues for solutions
- Review the troubleshooting section above

