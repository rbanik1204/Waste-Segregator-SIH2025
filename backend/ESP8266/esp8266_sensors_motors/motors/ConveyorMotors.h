/*
 * L298N Motor Driver #1 - Conveyor Belt Motors
 * Controls: Wet waste conveyor and Dry waste conveyor
 */

#ifndef CONVEYOR_MOTORS_H
#define CONVEYOR_MOTORS_H

#include <Arduino.h>

class ConveyorMotors {
  private:
    int wetIN1, wetIN2, wetENA;
    int dryIN3, dryIN4, dryENB;
    int speed; // PWM speed (0-255)
    bool wetActive;
    bool dryActive;
    
  public:
    ConveyorMotors(int in1, int in2, int ena, int in3, int in4, int enb);
    void begin();
    void startWet();
    void startDry();
    void stopWet();
    void stopDry();
    void stopAll();
    void setSpeed(int pwmSpeed);
    bool isWetActive();
    bool isDryActive();
};

#endif

