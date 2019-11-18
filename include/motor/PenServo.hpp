#include "Motor.hpp"
#include "Servo.h"

#ifndef _HPP_PENSERVO
#define _HPP_PENSERVO

class PenServo: public Motor {
    int pin;
    int angle = -1;
    int lastAngle;
    int maxAngle;
    Servo servo;
    bool inverted;
    int radius = 17; // in mm, we don't need to be super accurate
    float fullRotationTime;
    unsigned long lastMove = 0;

public:
    PenServo(int pinNumber, int maxAngle, float fullRotationTime, bool inverted = false);

    Milimeter getPosition() override;
    Milimeter getTargetPosition() override;
    void setTargetPosition(Milimeter position, bool relative = false) override;
    Milimeter getSpeed() override;
    void setSpeed(Milimeter speed) override;
    int getPin();
    
    int getAngle();
    void setAngle(int targetAngle);

    int getMaxAngle();
    void setMaxAngle(int angle);

    bool getInverted();
    void setInverted(bool target);

    void tick(unsigned long ms) override;

    void home() override;
    bool homed() override;
    void disable() override;
    void enable() override;
    bool enabled() override;
    bool moving() override;
};

#endif // _HPP_PENSERVO
