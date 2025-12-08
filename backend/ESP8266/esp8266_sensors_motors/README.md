# ESP8266: Sensors & Motor Control Module

## Purpose
This ESP8266 module handles **ALL sensors** and **motor control** for the waste segregation boat.

## Modular Structure
Each sensor and motor controller is in a separate file for easy maintenance and testing. See `MODULE_STRUCTURE.md` for details.

## Hardware Connected

### Sensors (All on ESP8266)
1. **MQ135 Air Quality Sensor** - NH₃, CO₂, NOx, C₆H₆, smoke, alcohol
2. **MQ2 Air Quality Sensor** - Smoke, alcohol, LPG, CH₄, benzene
3. **TDS Sensor** - Total Dissolved Solids (water health)
4. **DS18B20 Temperature Sensor** - Water temperature
5. **NEO-6M GPS Module** - Location tracking
6. **HMC5883L Compass** - Heading/direction (I2C)
7. **2x Capacitive Soil Moisture Sensors** - Dry bin & wet bin moisture
8. **Waterproof Ultrasonic Sensor** - Obstacle detection
9. **HX711 Load Cell** - Bin weight measurement

### Motor Drivers (L298N)
1. **L298N #1** - 2 Conveyor Belt Motors (wet & dry waste)
2. **L298N #2** - 2 Propeller Gear Motors (boat movement)

## Pin Configuration

### Air Quality Sensors
- MQ135: A0 (analog)
- MQ2: A0 (may need multiplexer if sharing ADC)

### TDS Sensor
- TDS: A0 (may need multiplexer)

### Temperature Sensor (DS18B20)
- Data: GPIO 4 (D4)

### GPS (NEO-6M)
- RX: GPIO 14 (D5)
- TX: GPIO 12 (D6)
- Baud: 9600

### Compass (HMC5883L) - I2C
- SDA: GPIO 4 (D2)
- SCL: GPIO 5 (D1)
- Address: 0x1E

### Moisture Sensors (Capacitive)
- Dry Bin: A0 (may need multiplexer)
- Wet Bin: A0 (may need multiplexer)

### Ultrasonic Sensor (Waterproof)
- Trig: GPIO 13 (D7)
- Echo: GPIO 15 (D8)

### Load Cell (HX711)
- DT: GPIO 4 (D2)
- SCK: GPIO 0 (D3)

### Motor Driver #1 - Conveyor Belts (L298N)
- Wet Motor:
  - IN1: GPIO 16 (D0)
  - IN2: GPIO 5 (D1)
  - ENA: GPIO 14 (D5)
- Dry Motor:
  - IN3: GPIO 4 (D2)
  - IN4: GPIO 0 (D3)
  - ENB: GPIO 12 (D6)

### Motor Driver #2 - Propellers (L298N)
- Propeller 1:
  - IN1: GPIO 3 (D9)
  - IN2: GPIO 1 (D10)
  - ENA: GPIO 7 (D13)
- Propeller 2:
  - IN3: GPIO 9 (D11)
  - IN4: GPIO 10 (D12)
  - ENB: GPIO 8 (D14)

## Required Libraries
Install via Arduino Library Manager:
- `ESP8266WiFi.h` (built-in)
- `ESP8266HTTPClient.h` (built-in)
- `SoftwareSerial.h` (built-in)
- `OneWire.h` - For DS18B20
- `DallasTemperature.h` - For DS18B20
- `HX711.h` - For load cell
- `Wire.h` (built-in) - For I2C compass
- `TinyGPS++.h` - For GPS parsing

## Configuration
Edit these in `config/WiFiConfig.h`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://192.168.1.100:4000/api/sensors/upload";
```

Edit pin definitions in `config/Pins.h` if needed.

## Calibration Required
1. **MQ135/MQ2**: Adjust calibration factors based on sensor readings
2. **TDS Sensor**: Calibrate based on known TDS solutions
3. **HX711 Load Cell**: Calibrate with known weights
4. **Moisture Sensors**: Adjust mapping based on sensor readings

## Data Transmission
- Sends telemetry every 1 second to backend
- JSON format with all sensor readings
- Includes motor status and boat mode

## Motor Control
- Commands can be sent via:
  - Serial monitor (for testing)
  - HTTP POST (from dashboard)
  - MQTT (if configured)
  - WebSocket (if configured)

## Notes
- **NO camera** on this module
- ESP32-CAM handles all image processing
- All sensors read continuously
- Motors controlled via L298N drivers
- Water health analysis based on TDS readings

## Water Health Zones (TDS-based)
- **Plastic Zone**: TDS < 50 ppm (low dissolved solids)
- **Normal Zone**: TDS 50-200 ppm
- **Moderate Pollution**: TDS 200-500 ppm
- **Chemical Zone**: TDS > 500 ppm (high dissolved solids)
- **Algal Bloom Risk**: High TDS + High Temperature

