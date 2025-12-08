/*
 * MQ135 Air Quality Sensor Implementation
 */

#include "MQ135.h"

MQ135::MQ135(int sensorPin, float calFactor) {
  pin = sensorPin;
  calibrationFactor = calFactor;
}

void MQ135::begin() {
  pinMode(pin, INPUT);
  Serial.println("MQ135 sensor initialized");
}

float MQ135::readPPM() {
  int sensorValue = analogRead(pin);
  // Convert analog reading to PPM
  // Formula: ppm = (sensorValue / 1024.0) * 5.0 * calibration_factor
  float voltage = (sensorValue / 1024.0) * 5.0;
  float ppm = voltage * calibrationFactor;
  return ppm;
}

void MQ135::setCalibrationFactor(float factor) {
  calibrationFactor = factor;
}

