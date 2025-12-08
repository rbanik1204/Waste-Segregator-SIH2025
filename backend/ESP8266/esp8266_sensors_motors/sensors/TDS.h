/*
 * TDS Sensor (Total Dissolved Solids)
 * Water health indicator: Plastic zones (low TDS) vs Chemical pollution (high TDS)
 */

#ifndef TDS_H
#define TDS_H

#include <Arduino.h>

class TDSSensor {
  private:
    int pin;
    float calibrationFactor; // Adjust based on sensor calibration
    
  public:
    TDSSensor(int sensorPin, float calFactor = 2.0);
    void begin();
    float readPPM();
    String getWaterHealthStatus(float tds);
    String getPollutionZone(float tds);
    void setCalibrationFactor(float factor);
};

#endif

