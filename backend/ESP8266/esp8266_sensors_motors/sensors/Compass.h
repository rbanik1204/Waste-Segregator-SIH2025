/*
 * HMC5883L Compass (Magnetometer)
 * Heading/direction for boat navigation
 */

#ifndef COMPASS_H
#define COMPASS_H

#include <Arduino.h>
#include <Wire.h>

class CompassSensor {
  private:
    int i2cAddress;
    
  public:
    CompassSensor(int address = 0x1E);
    void begin();
    float readHeading(); // Returns heading in degrees (0-360)
    bool isConnected();
};

#endif

