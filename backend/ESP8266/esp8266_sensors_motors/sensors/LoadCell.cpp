/*
 * HX711 Load Cell Implementation
 */

#include "LoadCell.h"

LoadCellSensor::LoadCellSensor(int dtPin, int sckPin, float calFactor) {
  scale = new HX711();
  scale->begin(dtPin, sckPin);
  calibrationFactor = calFactor;
}

void LoadCellSensor::begin() {
  scale->set_scale(calibrationFactor);
  scale->tare();
  Serial.println("HX711 load cell initialized");
}

float LoadCellSensor::readGrams() {
  if (scale->is_ready()) {
    float weight = scale->get_units(10); // Average of 10 readings
    if (weight < 0) weight = 0;
    return weight;
  }
  return 0.0;
}

void LoadCellSensor::tare() {
  scale->tare();
}

void LoadCellSensor::setCalibrationFactor(float factor) {
  calibrationFactor = factor;
  scale->set_scale(calibrationFactor);
}

bool LoadCellSensor::isReady() {
  return scale->is_ready();
}

