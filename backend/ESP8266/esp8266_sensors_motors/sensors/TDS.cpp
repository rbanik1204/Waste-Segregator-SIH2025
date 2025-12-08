/*
 * TDS Sensor Implementation
 */

#include "TDS.h"

TDSSensor::TDSSensor(int sensorPin, float calFactor) {
  pin = sensorPin;
  calibrationFactor = calFactor;
}

void TDSSensor::begin() {
  pinMode(pin, INPUT);
  Serial.println("TDS sensor initialized");
}

float TDSSensor::readPPM() {
  int sensorValue = analogRead(pin);
  // Convert to PPM
  // Formula: TDS(ppm) = (sensorValue * 3.3 / 1024.0) * 1000 / calibration_factor
  float voltage = sensorValue * 3.3 / 1024.0;
  float tds_ppm = voltage * 1000 / calibrationFactor;
  return tds_ppm;
}

String TDSSensor::getWaterHealthStatus(float tds) {
  if (tds < 50) return "Excellent";
  else if (tds < 200) return "Good";
  else if (tds < 500) return "Fair";
  else if (tds < 1000) return "Poor";
  else return "Critical";
}

String TDSSensor::getPollutionZone(float tds) {
  if (tds < 50) return "Plastic Zone (Low TDS)";
  else if (tds < 200) return "Normal Zone";
  else if (tds < 500) return "Moderate Pollution";
  else if (tds < 1000) return "Chemical Zone (High TDS)";
  else return "Critical Pollution Zone";
}

void TDSSensor::setCalibrationFactor(float factor) {
  calibrationFactor = factor;
}

