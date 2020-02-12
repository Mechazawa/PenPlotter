#include "motor/PenServo.hpp"
#include "Arduino.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PenServo::PenServo(int pinNumber, int maxAngle, float fullRotationTime, bool inverted) {
    this->pin = pinNumber;
    this->maxAngle = maxAngle;
    this->fullRotationTime = fullRotationTime;
    this->inverted = inverted;
}

Milimeter PenServo::getPosition() {
    if  (angle == lastAngle) {
        return angle;
    }

    int diff = angle > lastAngle ? angle - lastAngle : lastAngle - angle;
    float moveTime = (micros() - lastMove) / 1000000;
    Milimeter progress = diff * (moveTime / fullRotationTime);
    Milimeter position = progress + angle;
    Milimeter target = getTargetPosition();

    if  (position >= target) {
        angle = lastAngle;

        return target;
    }

    return position;
}

int PenServo::getPin() {
    return pin;
}

int PenServo::getAngle() {
    return inverted ? maxAngle - angle : angle;
}

void PenServo::setAngle(int targetAngle) {
    targetAngle = inverted ? maxAngle - targetAngle : targetAngle;
    targetAngle = targetAngle > maxAngle ? maxAngle : targetAngle;
    targetAngle = targetAngle < 0 ? 0 : targetAngle;

    if (!enabled()) {
        enable();
    }

    lastAngle = angle;
    servo.write(angle = targetAngle);
    lastMove = micros();
    // @todo hack
    delay(200);
}

int PenServo::getMaxAngle() {
    return maxAngle;
}

void PenServo::setMaxAngle(int angle) {
    maxAngle = angle;
}

bool PenServo::getInverted() {
    return inverted;
}

void PenServo::setInverted(bool target) {
    inverted = target;
}

Milimeter PenServo::getTargetPosition() {
    if (!homed()) {
        return -1;
    }

    Milimeter circumference = 2 * M_PI * radius;
    
    return circumference * (getAngle() / 360);
}

void PenServo::setTargetPosition(Milimeter position, bool relative) {
    if (relative && homed()) {
        position += getPosition();
    }

    setAngle(360 * (position / radius / (2 * M_PI)));
}

Milimeter PenServo::getSpeed() {
    Milimeter circumference = 2 * M_PI * radius;

    return circumference / fullRotationTime;
}

void PenServo::setSpeed(Milimeter speed) {
    // no-op
}

void PenServo::tick(const unsigned long ms) {
    // no-op
}

void PenServo::home() {
    setAngle(0);
}

bool PenServo::homed() {
    return angle >= 0;
}

void PenServo::disable() {
    servo.detach();
    angle = -1;
    lastAngle = -1;
}

void PenServo::enable() {
    servo.attach(pin);
    angle = -1;
    lastAngle = -1;
}

bool PenServo::enabled() {
    return servo.attached();
}

bool PenServo::moving() {
    return angle != lastAngle;
}