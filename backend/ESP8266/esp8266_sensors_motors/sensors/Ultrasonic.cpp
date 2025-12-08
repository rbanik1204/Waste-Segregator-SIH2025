/*
 * Waterproof Ultrasonic Sensor Implementation
 */

#include "Ultrasonic.h"

UltrasonicSensor::UltrasonicSensor(int triggerPin, int echoPin) {
  trigPin = triggerPin;
  echoPin = echoPin;
}

void UltrasonicSensor::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Ultrasonic sensor initialized");
}

float UltrasonicSensor::readDistanceCM() {
  // Send trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo pulse
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm
  // Speed of sound = 340 m/s = 0.034 cm/μs
  float distance = duration * 0.034 / 2;
  
  return distance;
}

bool UltrasonicSensor::isObstacleDetected(float thresholdCM) {
  float distance = readDistanceCM();
  return distance < thresholdCM && distance > 0;
}

