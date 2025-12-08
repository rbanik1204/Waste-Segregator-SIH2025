/*
 * MQ2 Air Quality Sensor Implementation
 */

#include "MQ2.h"

MQ2::MQ2(int sensorPin, float calFactor) {
  pin = sensorPin;
  calibrationFactor = calFactor;
}

void MQ2::begin() {
  pinMode(pin, INPUT);
  Serial.println("MQ2 sensor initialized");
}

float MQ2::readPPM() {
  int sensorValue = analogRead(pin);
  // Convert analog reading to PPM
  float voltage = (sensorValue / 1024.0) * 5.0;
  float ppm = voltage * calibrationFactor;
  return ppm;
}

void MQ2::setCalibrationFactor(float factor) {
  calibrationFactor = factor;
}

