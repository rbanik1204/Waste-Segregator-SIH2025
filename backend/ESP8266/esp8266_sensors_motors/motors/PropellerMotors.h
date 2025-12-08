/*
 * L298N Motor Driver #2 - Propeller Motors
 * Controls: 2 propeller gear motors for boat movement
 */

#ifndef PROPELLER_MOTORS_H
#define PROPELLER_MOTORS_H

#include <Arduino.h>

class PropellerMotors {
  private:
    int prop1IN1, prop1IN2, prop1ENA;
    int prop2IN3, prop2IN4, prop2ENB;
    int speed; // PWM speed (0-255)
    bool active;
    
  public:
    PropellerMotors(int in1, int in2, int ena, int in3, int in4, int enb);
    void begin();
    void moveForward();
    void moveLeft();
    void moveRight();
    void moveBackward();
    void stop();
    void setSpeed(int pwmSpeed);
    bool isActive();
};

#endif

