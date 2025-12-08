/*
 * Pin Definitions for ESP8266
 * Centralized pin configuration for all sensors and motors
 */

#ifndef PINS_H
#define PINS_H

// === AIR QUALITY SENSORS ===
#define MQ135_PIN A0      // Analog pin for MQ135
#define MQ2_PIN A0        // Analog pin for MQ2 (may need multiplexer)

// === TDS SENSOR ===
#define TDS_PIN A0        // Analog pin for TDS (may need multiplexer)

// === TEMPERATURE SENSOR (DS18B20) ===
#define ONE_WIRE_BUS D4   // GPIO 4 (D4 on NodeMCU)

// === GPS (NEO-6M) ===
#define GPS_RX_PIN D5     // GPIO 14
#define GPS_TX_PIN D6     // GPIO 12

// === COMPASS (HMC5883L) - I2C ===
// SDA = D2 (GPIO 4)
// SCL = D1 (GPIO 5)
#define COMPASS_I2C_ADDR 0x1E

// === MOISTURE SENSORS (Capacitive) ===
#define MOISTURE_DRY_PIN A0  // Dry bin moisture sensor
#define MOISTURE_WET_PIN A0  // Wet bin moisture sensor (may need multiplexer)

// === ULTRASONIC SENSOR (Waterproof) ===
#define ULTRASONIC_TRIG_PIN D7  // GPIO 13
#define ULTRASONIC_ECHO_PIN D8  // GPIO 15

// === LOAD CELL (HX711) ===
#define HX711_DT_PIN D2   // GPIO 4
#define HX711_SCK_PIN D3  // GPIO 0

// === MOTOR DRIVER #1 - CONVEYOR BELTS (L298N) ===
#define CONVEYOR_IN1 D0   // GPIO 16 - Wet conveyor motor 1
#define CONVEYOR_IN2 D1   // GPIO 5  - Wet conveyor motor 2
#define CONVEYOR_IN3 D2   // GPIO 4  - Dry conveyor motor 1
#define CONVEYOR_IN4 D3   // GPIO 0  - Dry conveyor motor 2
#define CONVEYOR_ENA D5   // GPIO 14 - Enable pin for wet conveyor
#define CONVEYOR_ENB D6   // GPIO 12 - Enable pin for dry conveyor

// === MOTOR DRIVER #2 - PROPELLERS (L298N) ===
#define PROPELLER_IN1 D9   // GPIO 3  - Propeller motor 1
#define PROPELLER_IN2 D10  // GPIO 1  - Propeller motor 2
#define PROPELLER_IN3 D11  // GPIO 9  - Propeller motor 3
#define PROPELLER_IN4 D12  // GPIO 10 - Propeller motor 4
#define PROPELLER_ENA D13  // GPIO 7  - Enable pin for propeller 1
#define PROPELLER_ENB D14  // GPIO 8  - Enable pin for propeller 2

#endif

