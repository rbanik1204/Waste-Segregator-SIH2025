/*
 * DS18B20 Temperature Sensor Implementation
 */

#include "Temperature.h"

TemperatureSensor::TemperatureSensor(int pin) {
  oneWire = new OneWire(pin);
  sensor = new DallasTemperature(oneWire);
}

void TemperatureSensor::begin() {
  sensor->begin();
  Serial.println("DS18B20 temperature sensor initialized");
}

float TemperatureSensor::readCelsius() {
  sensor->requestTemperatures();
  float temp = sensor->getTempCByIndex(0);
  if (temp == -127.0) {
    return 0; // Error reading
  }
  return temp;
}

bool TemperatureSensor::isConnected() {
  return sensor->getDeviceCount() > 0;
}

