/*
 * ESP8266: All Sensors + Motor Control
 * 
 * Main file - Uses modular sensor and motor classes
 * 
 * Hardware: ESP8266 NodeMCU or similar
 * Purpose: Read all sensors and control motors via L298N drivers
 */

// === INCLUDES ===
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Configuration
#include "config/Pins.h"
#include "config/WiFiConfig.h"

// Sensors
#include "sensors/MQ135.h"
#include "sensors/MQ2.h"
#include "sensors/TDS.h"
#include "sensors/Temperature.h"
#include "sensors/GPS.h"
#include "sensors/Compass.h"
#include "sensors/Moisture.h"
#include "sensors/Ultrasonic.h"
#include "sensors/LoadCell.h"

// Motors
#include "motors/ConveyorMotors.h"
#include "motors/PropellerMotors.h"

// === SENSOR OBJECTS ===
MQ135 mq135(MQ135_PIN, 200.0);
MQ2 mq2(MQ2_PIN, 200.0);
TDSSensor tdsSensor(TDS_PIN, 2.0);
TemperatureSensor tempSensor(ONE_WIRE_BUS);
GPSSensor gpsSensor(GPS_RX_PIN, GPS_TX_PIN);
CompassSensor compassSensor(COMPASS_I2C_ADDR);
MoistureSensor moistureDry(MOISTURE_DRY_PIN);
MoistureSensor moistureWet(MOISTURE_WET_PIN);
UltrasonicSensor ultrasonic(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
LoadCellSensor loadCell(HX711_DT_PIN, HX711_SCK_PIN, 2280.0);

// === MOTOR OBJECTS ===
ConveyorMotors conveyorMotors(
  CONVEYOR_IN1, CONVEYOR_IN2, CONVEYOR_ENA,
  CONVEYOR_IN3, CONVEYOR_IN4, CONVEYOR_ENB
);

PropellerMotors propellerMotors(
  PROPELLER_IN1, PROPELLER_IN2, PROPELLER_ENA,
  PROPELLER_IN3, PROPELLER_IN4, PROPELLER_ENB
);

// === GLOBAL VARIABLES ===
unsigned long lastTelemetry = 0;
const unsigned long TELEMETRY_INTERVAL = 1000; // Send data every 1 second

// Sensor readings
float mq135_ppm = 0;
float mq2_ppm = 0;
float tds_ppm = 0;
float temperature_c = 0;
float latitude = 0;
float longitude = 0;
float heading_deg = 0;
int moisture_dry_pct = 0;
int moisture_wet_pct = 0;
float ultrasonic_cm = 0;
float loadcell_grams = 0;

// Boat mode
String boat_mode = "manual"; // "autonomous" or "manual"

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP8266: Sensors & Motor Control Module ===");
  
  // Initialize I2C for compass
  Wire.begin();
  
  // Initialize all sensors
  Serial.println("Initializing sensors...");
  mq135.begin();
  mq2.begin();
  tdsSensor.begin();
  tempSensor.begin();
  gpsSensor.begin();
  compassSensor.begin();
  moistureDry.begin();
  moistureWet.begin();
  ultrasonic.begin();
  loadCell.begin();
  
  // Initialize motors
  Serial.println("Initializing motors...");
  conveyorMotors.begin();
  propellerMotors.begin();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 60) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }
  
  Serial.println("All systems initialized");
  Serial.println("==========================================\n");
}

void loop() {
  // Update GPS (needs continuous reading)
  gpsSensor.update();
  
  // Read all sensors
  mq135_ppm = mq135.readPPM();
  mq2_ppm = mq2.readPPM();
  tds_ppm = tdsSensor.readPPM();
  temperature_c = tempSensor.readCelsius();
  latitude = gpsSensor.getLatitude();
  longitude = gpsSensor.getLongitude();
  heading_deg = compassSensor.readHeading();
  moisture_dry_pct = moistureDry.readPercentage();
  moisture_wet_pct = moistureWet.readPercentage();
  ultrasonic_cm = ultrasonic.readDistanceCM();
  loadcell_grams = loadCell.readGrams();
  
  // Send telemetry to backend
  if (millis() - lastTelemetry >= TELEMETRY_INTERVAL) {
    lastTelemetry = millis();
    sendTelemetry();
  }
  
  // Check for motor commands
  checkMotorCommands();
  
  // Check for obstacles
  if (ultrasonic.isObstacleDetected(25.0)) {
    Serial.println("⚠️  Obstacle detected! Stopping propellers...");
    propellerMotors.stop();
  }
  
  delay(100); // Small delay
}

void sendTelemetry() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  
  // Build JSON payload
  String json = "{";
  json += "\"device_id\":\"esp8266_1\",";
  json += "\"boat_id\":\"boat_1\",";
  json += "\"lat\":" + String(latitude, 6) + ",";
  json += "\"lon\":" + String(longitude, 6) + ",";
  json += "\"heading_deg\":" + String(heading_deg, 2) + ",";
  json += "\"mq135_ppm\":" + String(mq135_ppm, 2) + ",";
  json += "\"mq2_ppm\":" + String(mq2_ppm, 2) + ",";
  json += "\"soil_dry_belt_pct\":" + String(moisture_dry_pct) + ",";
  json += "\"soil_wet_belt_pct\":" + String(moisture_wet_pct) + ",";
  json += "\"loadcell_grams\":" + String(loadcell_grams, 2) + ",";
  json += "\"tds_ppm\":" + String(tds_ppm, 2) + ",";
  json += "\"ultrasonic_cm\":" + String(ultrasonic_cm, 2) + ",";
  json += "\"temperature_c\":" + String(temperature_c, 2) + ",";
  json += "\"battery_volt\":12.6,"; // Add battery monitoring if available
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"conveyor_wet_active\":" + String(conveyorMotors.isWetActive() ? "true" : "false") + ",";
  json += "\"conveyor_dry_active\":" + String(conveyorMotors.isDryActive() ? "true" : "false") + ",";
  json += "\"propeller_active\":" + String(propellerMotors.isActive() ? "true" : "false") + ",";
  json += "\"mode\":\"" + boat_mode + "\"";
  json += "}";
  
  int httpCode = http.POST(json);
  if (httpCode > 0) {
    Serial.printf("Telemetry sent: %d\n", httpCode);
  } else {
    Serial.printf("Telemetry send failed: %d\n", httpCode);
  }
  
  http.end();
}

void checkMotorCommands() {
  // Check for commands via Serial (for testing) or HTTP
  // In production, use MQTT or WebSocket for real-time commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd == "forward") {
      propellerMotors.moveForward();
      Serial.println("Moving forward");
    }
    else if (cmd == "left") {
      propellerMotors.moveLeft();
      Serial.println("Turning left");
    }
    else if (cmd == "right") {
      propellerMotors.moveRight();
      Serial.println("Turning right");
    }
    else if (cmd == "stop") {
      propellerMotors.stop();
      Serial.println("Stopped");
    }
    else if (cmd == "wet_on") {
      conveyorMotors.startWet();
      Serial.println("Wet conveyor ON");
    }
    else if (cmd == "wet_off") {
      conveyorMotors.stopWet();
      Serial.println("Wet conveyor OFF");
    }
    else if (cmd == "dry_on") {
      conveyorMotors.startDry();
      Serial.println("Dry conveyor ON");
    }
    else if (cmd == "dry_off") {
      conveyorMotors.stopDry();
      Serial.println("Dry conveyor OFF");
    }
    else if (cmd == "autonomous") {
      boat_mode = "autonomous";
      Serial.println("Autonomous mode enabled");
    }
    else if (cmd == "manual") {
      boat_mode = "manual";
      Serial.println("Manual mode enabled");
    }
  }
}
