/*
 * HX711 Load Cell
 * Bin weight measurement
 */

#ifndef LOADCELL_H
#define LOADCELL_H

#include <Arduino.h>
#include <HX711.h>

class LoadCellSensor {
  private:
    HX711* scale;
    float calibrationFactor;
    
  public:
    LoadCellSensor(int dtPin, int sckPin, float calFactor = 2280.0);
    void begin();
    float readGrams(); // Returns weight in grams
    void tare(); // Zero the scale
    void setCalibrationFactor(float factor);
    bool isReady();
};

#endif

