/*
 * HMC5883L Compass Implementation
 */

#include "Compass.h"

CompassSensor::CompassSensor(int address) {
  i2cAddress = address;
}

void CompassSensor::begin() {
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x02); // Mode register
  Wire.write(0x00); // Continuous measurement mode
  Wire.endTransmission();
  Serial.println("HMC5883L compass initialized");
}

float CompassSensor::readHeading() {
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x03); // Register for data
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress, 6);
  
  if (Wire.available() >= 6) {
    int16_t x = (Wire.read() << 8) | Wire.read();
    int16_t z = (Wire.read() << 8) | Wire.read();
    int16_t y = (Wire.read() << 8) | Wire.read();
    
    // Calculate heading in degrees
    float heading = atan2(y, x) * 180.0 / PI;
    if (heading < 0) heading += 360;
    return heading;
  }
  return 0.0;
}

bool CompassSensor::isConnected() {
  Wire.beginTransmission(i2cAddress);
  return Wire.endTransmission() == 0;
}

