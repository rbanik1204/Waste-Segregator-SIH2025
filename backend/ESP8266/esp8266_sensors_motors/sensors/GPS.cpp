/*
 * NEO-6M GPS Module Implementation
 */

#include "GPS.h"

GPSSensor::GPSSensor(int rxPin, int txPin) {
  gpsSerial = new SoftwareSerial(rxPin, txPin);
  gps = new TinyGPSPlus();
}

void GPSSensor::begin() {
  gpsSerial->begin(9600);
  Serial.println("NEO-6M GPS module initialized");
}

void GPSSensor::update() {
  while (gpsSerial->available() > 0) {
    if (gps->encode(gpsSerial->read())) {
      // GPS data decoded
    }
  }
}

float GPSSensor::getLatitude() {
  if (gps->location.isValid()) {
    return gps->location.lat();
  }
  return 0.0;
}

float GPSSensor::getLongitude() {
  if (gps->location.isValid()) {
    return gps->location.lng();
  }
  return 0.0;
}

bool GPSSensor::isValid() {
  return gps->location.isValid();
}

float GPSSensor::getSpeed() {
  if (gps->speed.isValid()) {
    return gps->speed.mps(); // meters per second
  }
  return 0.0;
}

int GPSSensor::getSatellites() {
  if (gps->satellites.isValid()) {
    return gps->satellites.value();
  }
  return 0;
}

