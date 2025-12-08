/*
 * DS18B20 Temperature Sensor
 * Water temperature monitoring
 */

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TemperatureSensor {
  private:
    OneWire* oneWire;
    DallasTemperature* sensor;
    
  public:
    TemperatureSensor(int pin);
    void begin();
    float readCelsius();
    bool isConnected();
};

#endif

