# ESP8266 Module Structure

## File Organization

```
esp8266_sensors_motors/
├── esp8266_sensors_motors.ino    (Main file)
│
├── config/
│   ├── Pins.h                     (Pin definitions)
│   └── WiFiConfig.h               (WiFi configuration)
│
├── sensors/
│   ├── MQ135.h / MQ135.cpp        (Air quality - NH₃, CO₂, NOx, etc.)
│   ├── MQ2.h / MQ2.cpp            (Air quality - Smoke, LPG, etc.)
│   ├── TDS.h / TDS.cpp            (Water health sensor)
│   ├── Temperature.h / Temperature.cpp  (DS18B20)
│   ├── GPS.h / GPS.cpp            (NEO-6M GPS)
│   ├── Compass.h / Compass.cpp    (HMC5883L)
│   ├── Moisture.h / Moisture.cpp  (Capacitive sensors)
│   ├── Ultrasonic.h / Ultrasonic.cpp  (Waterproof ultrasonic)
│   └── LoadCell.h / LoadCell.cpp  (HX711)
│
└── motors/
    ├── ConveyorMotors.h / ConveyorMotors.cpp  (L298N #1)
    └── PropellerMotors.h / PropellerMotors.cpp  (L298N #2)
```

## Sensor Modules

### 1. MQ135 (Air Quality)
- **File**: `sensors/MQ135.h` / `MQ135.cpp`
- **Detects**: NH₃, CO₂, NOx, C₆H₆, smoke, alcohol
- **Methods**: `begin()`, `readPPM()`, `setCalibrationFactor()`

### 2. MQ2 (Air Quality)
- **File**: `sensors/MQ2.h` / `MQ2.cpp`
- **Detects**: Smoke, alcohol, LPG, CH₄, benzene
- **Methods**: `begin()`, `readPPM()`, `setCalibrationFactor()`

### 3. TDS Sensor (Water Health)
- **File**: `sensors/TDS.h` / `TDS.cpp`
- **Purpose**: Total Dissolved Solids - water quality indicator
- **Methods**: `begin()`, `readPPM()`, `getWaterHealthStatus()`, `getPollutionZone()`

### 4. Temperature Sensor (DS18B20)
- **File**: `sensors/Temperature.h` / `Temperature.cpp`
- **Purpose**: Water temperature monitoring
- **Methods**: `begin()`, `readCelsius()`, `isConnected()`

### 5. GPS (NEO-6M)
- **File**: `sensors/GPS.h` / `GPS.cpp`
- **Purpose**: Location tracking
- **Methods**: `begin()`, `update()`, `getLatitude()`, `getLongitude()`, `getSpeed()`

### 6. Compass (HMC5883L)
- **File**: `sensors/Compass.h` / `Compass.cpp`
- **Purpose**: Heading/direction
- **Methods**: `begin()`, `readHeading()`, `isConnected()`

### 7. Moisture Sensors (Capacitive)
- **File**: `sensors/Moisture.h` / `Moisture.cpp`
- **Purpose**: Dry bin & wet bin moisture levels
- **Methods**: `begin()`, `readPercentage()`, `calibrateDry()`, `calibrateWet()`

### 8. Ultrasonic Sensor
- **File**: `sensors/Ultrasonic.h` / `Ultrasonic.cpp`
- **Purpose**: Obstacle detection
- **Methods**: `begin()`, `readDistanceCM()`, `isObstacleDetected()`

### 9. Load Cell (HX711)
- **File**: `sensors/LoadCell.h` / `LoadCell.cpp`
- **Purpose**: Bin weight measurement
- **Methods**: `begin()`, `readGrams()`, `tare()`, `setCalibrationFactor()`

## Motor Modules

### 1. Conveyor Motors (L298N #1)
- **File**: `motors/ConveyorMotors.h` / `ConveyorMotors.cpp`
- **Controls**: Wet & dry waste conveyor belts
- **Methods**: `begin()`, `startWet()`, `startDry()`, `stopWet()`, `stopDry()`, `stopAll()`

### 2. Propeller Motors (L298N #2)
- **File**: `motors/PropellerMotors.h` / `PropellerMotors.cpp`
- **Controls**: 2 propeller gear motors
- **Methods**: `begin()`, `moveForward()`, `moveLeft()`, `moveRight()`, `moveBackward()`, `stop()`

## Usage Example

```cpp
// In main .ino file
#include "sensors/MQ135.h"
MQ135 mq135(MQ135_PIN, 200.0);

void setup() {
  mq135.begin();
}

void loop() {
  float ppm = mq135.readPPM();
  Serial.println(ppm);
}
```

## Benefits of Modular Structure

1. **Easy Maintenance**: Each sensor in separate file
2. **Reusability**: Sensor classes can be reused
3. **Testing**: Test each sensor independently
4. **Clarity**: Clear separation of concerns
5. **Scalability**: Easy to add new sensors

## Adding New Sensors

1. Create header file: `sensors/NewSensor.h`
2. Create implementation: `sensors/NewSensor.cpp`
3. Include in main file: `#include "sensors/NewSensor.h"`
4. Create object and use in `setup()` and `loop()`

