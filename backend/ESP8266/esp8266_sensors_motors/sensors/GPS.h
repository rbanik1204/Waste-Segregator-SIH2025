/*
 * NEO-6M GPS Module
 * Location tracking for boat navigation
 */

#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

class GPSSensor {
  private:
    SoftwareSerial* gpsSerial;
    TinyGPSPlus* gps;
    
  public:
    GPSSensor(int rxPin, int txPin);
    void begin();
    void update();
    float getLatitude();
    float getLongitude();
    bool isValid();
    float getSpeed(); // m/s
    int getSatellites();
};

#endif

