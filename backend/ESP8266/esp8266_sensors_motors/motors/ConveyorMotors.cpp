/*
 * Conveyor Motors Implementation
 */

#include "ConveyorMotors.h"

ConveyorMotors::ConveyorMotors(int in1, int in2, int ena, int in3, int in4, int enb) {
  wetIN1 = in1;
  wetIN2 = in2;
  wetENA = ena;
  dryIN3 = in3;
  dryIN4 = in4;
  dryENB = enb;
  speed = 255; // Default full speed
  wetActive = false;
  dryActive = false;
}

void ConveyorMotors::begin() {
  pinMode(wetIN1, OUTPUT);
  pinMode(wetIN2, OUTPUT);
  pinMode(wetENA, OUTPUT);
  pinMode(dryIN3, OUTPUT);
  pinMode(dryIN4, OUTPUT);
  pinMode(dryENB, OUTPUT);
  stopAll();
  Serial.println("Conveyor motors initialized");
}

void ConveyorMotors::startWet() {
  digitalWrite(wetIN1, HIGH);
  digitalWrite(wetIN2, LOW);
  analogWrite(wetENA, speed);
  wetActive = true;
}

void ConveyorMotors::startDry() {
  digitalWrite(dryIN3, HIGH);
  digitalWrite(dryIN4, LOW);
  analogWrite(dryENB, speed);
  dryActive = true;
}

void ConveyorMotors::stopWet() {
  digitalWrite(wetIN1, LOW);
  digitalWrite(wetIN2, LOW);
  analogWrite(wetENA, 0);
  wetActive = false;
}

void ConveyorMotors::stopDry() {
  digitalWrite(dryIN3, LOW);
  digitalWrite(dryIN4, LOW);
  analogWrite(dryENB, 0);
  dryActive = false;
}

void ConveyorMotors::stopAll() {
  stopWet();
  stopDry();
}

void ConveyorMotors::setSpeed(int pwmSpeed) {
  speed = constrain(pwmSpeed, 0, 255);
  if (wetActive) analogWrite(wetENA, speed);
  if (dryActive) analogWrite(dryENB, speed);
}

bool ConveyorMotors::isWetActive() {
  return wetActive;
}

bool ConveyorMotors::isDryActive() {
  return dryActive;
}

