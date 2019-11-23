#include "motor/Motor.hpp"
#include "Position.hpp"
#include "Axis.hpp"
#include "Arduino.h"

double nthroot(double number, double n)
{
  if (n == 0) return NAN;
  if (number > 0) return pow(number, 1.0 / n);
  if (number == 0) return 0;
  if (number < 0 && int(n) == n && (int(n) & 1)) return -pow(-number, 1.0 / n);
  return NAN;
}

void Axis::setMotor(char axis, Motor* motor) {
    MotorNode* curr = motors;

    while (curr) {
        if (curr->axis == axis) {
            curr->motor = motor;

            return;
        }

        curr = curr->next;
    }

    MotorNode* newMotor = new MotorNode;
    newMotor->axis = axis;
    newMotor->motor = motor;
    newMotor->next = motors;

    motors = newMotor;
}

bool Axis::deleteMotor(char axis) {
    MotorNode* curr = motors;

    if (motors->axis == axis) {
        motors = curr->next;

        delete curr;
        return true;
    }

    MotorNode* last;

    while ((last = curr) && (curr = curr->next)) {
        if (curr->axis == axis) {
            last->next = curr->next;

            delete curr;
            return true;
        }
    }

    return false;
}

Motor* Axis::getMotor(char axis) {
    MotorNode* curr = motors;

    while (curr) {
        if (curr->axis == axis) {
            return curr->motor;
        }

        curr = curr->next;
    }

    return nullptr;
}

bool Axis::hasMotor(char axis) {
    return getMotor(axis) != nullptr;
}

void Axis::tick(unsigned long ms) {
    bool done = true;
    
    for (int i = 0; i < activeAxisCount; i++) {
        if (activeAxis[i]->moving()) {
            activeAxis[i]->tick(ms);

            done = false;
        }
    }

    if (done) {
        startNextMove();
    }
}

void Axis::startNextMove() {
    delete popMove();
    Movement* move = peekMove();

    activeAxisCount = move->position.getAxisCount();
    char axis[activeAxisCount];
    move->position.listAxis(axis);

    // Prepare motor lookup
    delete activeAxis;
    activeAxis = new Motor*[activeAxisCount];

    if (activeAxisCount == 0) {
        return;
    }

    Milimeter totalDistance = 0;
    Milimeter distances[activeAxisCount];

    for (int i = 0; i < activeAxisCount; i++) {
        activeAxis[i] = getMotor(axis[i]);

        // Set position
        Milimeter axisPos = move->position.getAxis(axis[i]);
        activeAxis[i]->setTargetPosition(axisPos);

        distances[i] = abs(axisPos - activeAxis[i]->getPosition());;
        totalDistance += pow(distances[i], 2);
    }

    totalDistance = nthroot(totalDistance, activeAxisCount);
    double time = totalDistance / move->unitsPerSecond;

    for (int i = 0; i < activeAxisCount; i++) {
        activeAxis[i]->setSpeed(distances[i] / time);
    }
} 

void Axis::tick() {
    tick(micros());
}

bool Axis::pushMove(Position position, Milimeter unitsPerSecond) {
    if (head == tail && stack[tail]) {
        return false;
    }

    unsigned short nextHead = (head + 1) % stackSize;
    
    stack[head] = new Movement;
    stack[head]->position = position;
    stack[head]->unitsPerSecond = unitsPerSecond;

    head = nextHead;

    return true;
}

Movement* Axis::popMove() {
    Movement* out = stack[tail];

    if (out == nullptr) {
        return nullptr;
    }

    stack[tail] = nullptr;
    tail = (tail + 1) % stackSize;

    return out;
}

Movement* Axis::peekMove() {
    return stack[tail];
}

unsigned short Axis::getStackSize() {
    if (head < tail) {
        return stackSize - tail + head;
    }

    if (head == tail && stack[tail]) {
        return getMaxStackSize();
    }

    return head - tail;   
}

unsigned short Axis::getMaxStackSize() {
    return stackSize;
}

void Axis::setMaxStackSize(unsigned short size) {
    Movement* newStack[size];

    for (unsigned short i = 0; i < size; i++) {
        newStack[i] = stack[(tail + i) % stackSize];
    }

    delete stack;

    stackSize = size;
    stack = newStack;
}