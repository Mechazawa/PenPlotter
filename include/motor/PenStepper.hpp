#include "Motor.hpp"

#ifndef _HPP_PENSTEPPER
#define _HPP_PENSTEPPER

class PenStepper : public Motor {
    short steps;
    Milimeter targetPosition;
    unsigned char stepPin;
    unsigned char directionPin;
    unsigned char enablePin;
    unsigned short stepsPerMM;
    unsigned short missedSteps = 0;
    Milimeter travelSpeed = 10;

    unsigned char currentDirection = 0;
    unsigned long lastStep = 0;
    unsigned char lastWrite = 0;

public:
    PenStepper(unsigned char stepPin, unsigned char directionPin, unsigned short stepsPerMM = 27, unsigned char enablePin = 8);

    Milimeter getPosition() override;
    Milimeter getTargetPosition() override;
    void setTargetPosition(Milimeter position, bool relative = false) override;
    Milimeter getSpeed() override;
    void setSpeed(Milimeter speed) override;
    unsigned int getStepsLeft();
    unsigned short getMissedSteps();
    void resetMissedSteps();

    void tick(const unsigned long ms) override;

    void home() override;
    bool homed() override;
    void disable() override;
    void enable() override;
    bool enabled() override;
    bool moving() override;
};

#endif // _HPP_PENSTEPPER
