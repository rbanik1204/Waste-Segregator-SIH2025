/*
 * MQ135 Air Quality Sensor
 * Detects: NH₃, CO₂, NOx, C₆H₆, smoke, alcohol
 */

#ifndef MQ135_H
#define MQ135_H

#include <Arduino.h>

class MQ135 {
  private:
    int pin;
    float calibrationFactor; // Adjust based on sensor calibration
    
  public:
    MQ135(int sensorPin, float calFactor = 200.0);
    void begin();
    float readPPM();
    void setCalibrationFactor(float factor);
};

#endif

