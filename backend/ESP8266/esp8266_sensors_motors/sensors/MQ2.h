/*
 * MQ2 Air Quality Sensor
 * Detects: Smoke, alcohol, LPG, CH₄, benzene
 */

#ifndef MQ2_H
#define MQ2_H

#include <Arduino.h>

class MQ2 {
  private:
    int pin;
    float calibrationFactor; // Adjust based on sensor calibration
    
  public:
    MQ2(int sensorPin, float calFactor = 200.0);
    void begin();
    float readPPM();
    void setCalibrationFactor(float factor);
};

#endif

