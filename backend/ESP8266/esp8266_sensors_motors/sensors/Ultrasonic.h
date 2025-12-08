/*
 * Waterproof Ultrasonic Sensor (JSN-SR04T or similar)
 * Obstacle detection for boat navigation
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class UltrasonicSensor {
  private:
    int trigPin;
    int echoPin;
    
  public:
    UltrasonicSensor(int triggerPin, int echoPin);
    void begin();
    float readDistanceCM(); // Returns distance in centimeters
    bool isObstacleDetected(float thresholdCM = 25.0); // Default 25cm threshold
};

#endif

