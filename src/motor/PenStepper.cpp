#include <Arduino.h>
#include "motor/PenStepper.hpp"

bool pullupPins[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

PenStepper::PenStepper(unsigned char stepPin, unsigned char directionPin, unsigned short stepsPerMM, unsigned char enablePin) {
    this->stepPin = stepPin;
    this->directionPin = directionPin;
    this->stepsPerMM = stepsPerMM;
    this->enablePin = enablePin;

    pinMode(stepPin, OUTPUT);
    pinMode(directionPin, OUTPUT);
}

Milimeter PenStepper::getPosition() {
    return (Milimeter)steps / stepsPerMM;
}

Milimeter PenStepper::getTargetPosition() {
    return targetPosition;
}

unsigned int PenStepper::getStepsLeft() {
    return round(abs(steps - (targetPosition * stepsPerMM)));
}

void PenStepper::setTargetPosition(Milimeter input, bool relative) {
    if (relative && homed()) {
        targetPosition = input + getPosition();
    } else {
        targetPosition = input;
    }
}

Milimeter PenStepper::getSpeed() {
    return travelSpeed;
}

void PenStepper::setSpeed(Milimeter speed) {
    travelSpeed = speed > 0 ? speed : 0;
}

unsigned short PenStepper::getMissedSteps() {
    return missedSteps;
}

void PenStepper::resetMissedSteps() {
    missedSteps = 0;
}

void PenStepper::tick(const unsigned long ms) {
    if(!enabled()) {
        return;
    }

    unsigned int stepsLeft = getStepsLeft();

    if (stepsLeft == 0) {
        lastStep = 0;
        targetPosition = getPosition();
        return;
    }

    unsigned char targetDirection = steps < (stepsPerMM * targetPosition) ? 1 : 0;
    unsigned int stepDelay = 1000000 / (travelSpeed * stepsPerMM);

    if (targetDirection == currentDirection) {
        unsigned short timeElapsed = ms - lastStep;

        if (timeElapsed >= stepDelay) {
            digitalWrite(stepPin, lastWrite = !lastWrite);

            if (lastStep > 0) {
                unsigned short missed = floor(timeElapsed / stepDelay) - 1;

                // @todo better catchup
                switch(missed) {
                    case 5:
                    delayMicroseconds(5);
                    digitalWrite(stepPin, lastWrite = !lastWrite);
                    case 4:
                    delayMicroseconds(5);
                    digitalWrite(stepPin, lastWrite = !lastWrite);
                    case 3:
                    delayMicroseconds(5);
                    digitalWrite(stepPin, lastWrite = !lastWrite);
                    case 2:
                    lastStep = ms + stepDelay / 3;
                    break;
                    case 1:
                    lastStep = ms + stepDelay / 2;
                    break;
                }

                missedSteps += missed;
            }

            lastStep = ms;

            if (currentDirection) {
                steps++;
            } else {
                steps--;
            }
        }
    } else {
        lastStep = stepDelay + ms - 50; // just wait 50ms
        digitalWrite (directionPin, currentDirection = targetDirection);
    }
}

void PenStepper::home() {
    digitalWrite (directionPin, currentDirection = 0);
    delay (50);

    for (int i = 0; i < 2300; i++) 
    {
        digitalWrite (stepPin, lastWrite = !lastWrite);
        delayMicroseconds (600);
    }

    steps = 0;
}

bool PenStepper::homed() {
    return steps == 0;
}

void PenStepper::disable() {
    digitalWrite (enablePin, HIGH);
    
    targetPosition = steps / stepsPerMM;

    pullupPins[enablePin] = false;
}

void PenStepper::enable() {
    if (!pullupPins[enablePin]) {
        pinMode(enablePin, OUTPUT);
        digitalWrite (enablePin, LOW);

        pullupPins[enablePin] = true;
    }
}

bool PenStepper::enabled() {
    return pullupPins[enablePin];
}

bool PenStepper::moving() {
    return steps != (targetPosition * stepsPerMM);
}