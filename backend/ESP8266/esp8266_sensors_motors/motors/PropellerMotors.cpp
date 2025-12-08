/*
 * Propeller Motors Implementation
 */

#include "PropellerMotors.h"

PropellerMotors::PropellerMotors(int in1, int in2, int ena, int in3, int in4, int enb) {
  prop1IN1 = in1;
  prop1IN2 = in2;
  prop1ENA = ena;
  prop2IN3 = in3;
  prop2IN4 = in4;
  prop2ENB = enb;
  speed = 255; // Default full speed
  active = false;
}

void PropellerMotors::begin() {
  pinMode(prop1IN1, OUTPUT);
  pinMode(prop1IN2, OUTPUT);
  pinMode(prop1ENA, OUTPUT);
  pinMode(prop2IN3, OUTPUT);
  pinMode(prop2IN4, OUTPUT);
  pinMode(prop2ENB, OUTPUT);
  stop();
  Serial.println("Propeller motors initialized");
}

void PropellerMotors::moveForward() {
  digitalWrite(prop1IN1, HIGH);
  digitalWrite(prop1IN2, LOW);
  digitalWrite(prop2IN3, HIGH);
  digitalWrite(prop2IN4, LOW);
  analogWrite(prop1ENA, speed);
  analogWrite(prop2ENB, speed);
  active = true;
}

void PropellerMotors::moveLeft() {
  // Left motor reverse, right motor forward
  digitalWrite(prop1IN1, LOW);
  digitalWrite(prop1IN2, HIGH);
  digitalWrite(prop2IN3, HIGH);
  digitalWrite(prop2IN4, LOW);
  analogWrite(prop1ENA, speed * 0.8); // Slightly slower for turning
  analogWrite(prop2ENB, speed * 0.8);
  active = true;
}

void PropellerMotors::moveRight() {
  // Left motor forward, right motor reverse
  digitalWrite(prop1IN1, HIGH);
  digitalWrite(prop1IN2, LOW);
  digitalWrite(prop2IN3, LOW);
  digitalWrite(prop2IN4, HIGH);
  analogWrite(prop1ENA, speed * 0.8);
  analogWrite(prop2ENB, speed * 0.8);
  active = true;
}

void PropellerMotors::moveBackward() {
  digitalWrite(prop1IN1, LOW);
  digitalWrite(prop1IN2, HIGH);
  digitalWrite(prop2IN3, LOW);
  digitalWrite(prop2IN4, HIGH);
  analogWrite(prop1ENA, speed);
  analogWrite(prop2ENB, speed);
  active = true;
}

void PropellerMotors::stop() {
  digitalWrite(prop1IN1, LOW);
  digitalWrite(prop1IN2, LOW);
  digitalWrite(prop2IN3, LOW);
  digitalWrite(prop2IN4, LOW);
  analogWrite(prop1ENA, 0);
  analogWrite(prop2ENB, 0);
  active = false;
}

void PropellerMotors::setSpeed(int pwmSpeed) {
  speed = constrain(pwmSpeed, 0, 255);
  if (active) {
    analogWrite(prop1ENA, speed);
    analogWrite(prop2ENB, speed);
  }
}

bool PropellerMotors::isActive() {
  return active;
}

