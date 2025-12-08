/*
 * Capacitive Soil Moisture Sensors
 * Two sensors: Dry bin and Wet bin moisture levels
 */

#ifndef MOISTURE_H
#define MOISTURE_H

#include <Arduino.h>

class MoistureSensor {
  private:
    int pin;
    int dryValue;  // Sensor reading in dry condition
    int wetValue;  // Sensor reading in wet condition
    
  public:
    MoistureSensor(int sensorPin, int dry = 0, int wet = 1024);
    void begin();
    int readPercentage(); // Returns 0-100%
    void calibrateDry();  // Calibrate for dry condition
    void calibrateWet();  // Calibrate for wet condition
    void setCalibration(int dry, int wet);
};

#endif

