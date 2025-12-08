/*
 * Capacitive Soil Moisture Sensor Implementation
 */

#include "Moisture.h"

MoistureSensor::MoistureSensor(int sensorPin, int dry, int wet) {
  pin = sensorPin;
  dryValue = dry;
  wetValue = wet;
}

void MoistureSensor::begin() {
  pinMode(pin, INPUT);
  Serial.println("Moisture sensor initialized");
}

int MoistureSensor::readPercentage() {
  int sensorValue = analogRead(pin);
  // Map sensor value to percentage
  int percentage = map(sensorValue, dryValue, wetValue, 0, 100);
  // Clamp to 0-100
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  return percentage;
}

void MoistureSensor::calibrateDry() {
  dryValue = analogRead(pin);
  Serial.print("Dry calibration: ");
  Serial.println(dryValue);
}

void MoistureSensor::calibrateWet() {
  wetValue = analogRead(pin);
  Serial.print("Wet calibration: ");
  Serial.println(wetValue);
}

void MoistureSensor::setCalibration(int dry, int wet) {
  dryValue = dry;
  wetValue = wet;
}

