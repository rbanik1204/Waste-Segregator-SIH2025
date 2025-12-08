# Arduino/ESP Code Setup Guide

## Project Structure

```
backend/
├── ESP32/
│   └── esp32cam_image_processing/
│       ├── esp32cam_image_processing.ino  (Image processing ONLY)
│       └── README.md
│
└── ESP8266/
    └── esp8266_sensors_motors/
        ├── esp8266_sensors_motors.ino     (ALL sensors + motors)
        └── README.md
```

## Hardware Overview

### ESP32-CAM Module
**Purpose**: Image processing only
- Camera: OV2640
- Function: Capture images → Send to FastAPI → Receive classification
- **NO sensors connected**

### ESP8266 Module
**Purpose**: All sensors + motor control
- Sensors: MQ135, MQ2, TDS, DS18B20, GPS, Compass, Moisture (x2), Ultrasonic, Load Cell
- Motors: 2x Conveyor (L298N #1), 2x Propeller (L298N #2)
- **NO camera**

## Installation Steps

### 1. Install Arduino IDE
Download from: https://www.arduino.cc/en/software

### 2. Install Board Support

#### For ESP32-CAM:
1. File → Preferences
2. Add to Additional Board Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Tools → Board → Boards Manager
4. Search "ESP32" and install "esp32 by Espressif Systems"
5. Select board: **AI Thinker ESP32-CAM**

#### For ESP8266:
1. File → Preferences
2. Add to Additional Board Manager URLs:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Tools → Board → Boards Manager
4. Search "ESP8266" and install "esp8266 by ESP8266 Community"
5. Select board: **NodeMCU 1.0 (ESP-12E Module)**

### 3. Install Required Libraries

Open Tools → Manage Libraries and install:

#### For ESP32-CAM:
- `ArduinoJson` (optional, for JSON parsing)

#### For ESP8266:
- `OneWire` - For DS18B20 temperature sensor
- `DallasTemperature` - For DS18B20
- `HX711` - For load cell
- `TinyGPS++` - For GPS parsing

### 4. Configure WiFi & Server

Edit both `.ino` files and update:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://YOUR_SERVER_IP:PORT";
```

### 5. Upload Code

1. Connect ESP32-CAM via USB (may need FTDI adapter)
2. Select correct board and port
3. Upload `esp32cam_image_processing.ino`
4. Repeat for ESP8266 with `esp8266_sensors_motors.ino`

## Wiring Reference

### ESP8266 Sensor Connections

| Sensor | Pin | Notes |
|--------|-----|-------|
| MQ135 | A0 | Analog pin |
| MQ2 | A0 | May need multiplexer |
| TDS | A0 | May need multiplexer |
| DS18B20 | D4 | OneWire bus |
| GPS RX | D5 | Software Serial |
| GPS TX | D6 | Software Serial |
| Compass SDA | D2 | I2C |
| Compass SCL | D1 | I2C |
| Moisture Dry | A0 | May need multiplexer |
| Moisture Wet | A0 | May need multiplexer |
| Ultrasonic Trig | D7 | |
| Ultrasonic Echo | D8 | |
| HX711 DT | D2 | |
| HX711 SCK | D3 | |

### Motor Connections

#### L298N #1 (Conveyor Belts)
- Wet Motor: IN1/IN2/ENA → D0/D1/D5
- Dry Motor: IN3/IN4/ENB → D2/D3/D6

#### L298N #2 (Propellers)
- Propeller 1: IN1/IN2/ENA → D9/D10/D13
- Propeller 2: IN3/IN4/ENB → D11/D12/D14

## Calibration

### MQ135/MQ2 Sensors
1. Expose to clean air
2. Note reading
3. Adjust calibration factor in code:
   ```cpp
   mq135_ppm = (sensorValue / 1024.0) * 5.0 * calibration_factor;
   ```

### TDS Sensor
1. Test with known TDS solution (e.g., 1413 ppm calibration solution)
2. Adjust formula:
   ```cpp
   tds_ppm = voltage * 1000 / calibration_factor;
   ```

### HX711 Load Cell
1. Place known weight on load cell
2. Read raw value
3. Calculate scale factor:
   ```cpp
   scale.set_scale(calibration_factor);
   ```

### Moisture Sensors
1. Test in dry soil (0% moisture)
2. Test in water (100% moisture)
3. Adjust mapping:
   ```cpp
   moisture_pct = map(sensorValue, dryValue, wetValue, 0, 100);
   ```

## Testing

### ESP32-CAM
1. Open Serial Monitor (115200 baud)
2. Check WiFi connection
3. Verify image capture messages
4. Check for classification responses

### ESP8266
1. Open Serial Monitor (115200 baud)
2. Check WiFi connection
3. Verify sensor readings
4. Test motor commands via Serial:
   - Type "forward", "left", "right", "stop"
   - Type "wet_on", "wet_off", "dry_on", "dry_off"

## Troubleshooting

### ESP32-CAM Issues
- **Camera init failed**: Check pin connections, try different board variant
- **WiFi not connecting**: Check SSID/password, signal strength
- **Image send failed**: Check server URL, network connectivity

### ESP8266 Issues
- **Sensor readings wrong**: Calibrate sensors, check wiring
- **Motors not working**: Check L298N connections, power supply
- **GPS not working**: Check antenna connection, wait for satellite lock

## Integration with Backend

Both modules send data to:
- **ESP32-CAM**: `http://SERVER:8000/image` (FastAPI)
- **ESP8266**: `http://SERVER:4000/api/sensors/upload` (Node.js backend)

Dashboard receives data via:
- WebSocket (real-time)
- HTTP REST API (polling)

## Next Steps

1. Upload both codes
2. Verify sensor readings in Serial Monitor
3. Check dashboard for incoming data
4. Calibrate sensors based on real-world readings
5. Test motor control from dashboard

---

**Note**: Pin assignments may vary based on your specific ESP8266 board. Adjust GPIO numbers accordingly.

