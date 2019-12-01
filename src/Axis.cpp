#include "motor/Motor.hpp"
#include "Position.hpp"
#include "Axis.hpp"
#include "Arduino.h"

float diff(float a, float b) {
    if (a > b) {
        return a - b;
    }
    
    return b - a;
}

Axis::Axis(unsigned short stackSize) {
    setMaxStackSize(stackSize);
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

    while ((curr = curr->next) != nullptr) {
        if (curr->axis == axis) {
            last->next = curr->next;

            delete curr;
            return true;
        }

        last = curr;
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

bool Axis::moving() {
    return activeAxisCount > 0;
}

void Axis::startNextMove() {
    if(currentMove != nullptr) {
        Movement* lastMove = popMove();
        delete lastMove->position;
        delete lastMove;
    }

    currentMove = peekMove();

    if (currentMove == nullptr) {
        activeAxisCount = 0;   

        return;
    }

    activeAxisCount = currentMove->position->getAxisCount();

    char axis[activeAxisCount];
    currentMove->position->listAxis(axis);

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
        Milimeter axisPos = currentMove->position->getAxis(axis[i]);
        activeAxis[i]->setTargetPosition(axisPos);

        distances[i] = diff(axisPos, activeAxis[i]->getPosition());

        totalDistance += powf(distances[i], 2);
    }

    totalDistance = totalDistance == 0 ? 0 : sqrtf(totalDistance);
    double time = totalDistance / currentMove->unitsPerSecond;

    for (int i = 0; i < activeAxisCount; i++) {
        Milimeter speed = time == 0 ? 0 : (distances[i] / time);
        activeAxis[i]->setSpeed(speed);
    }
} 

void Axis::tick() {
    tick(micros());
}

bool Axis::pushMove(Position* position, Milimeter unitsPerSecond) {
    if (stack[head] != nullptr) {
        return false;
    }

    stack[head] = new Movement;
    stack[head]->position = position;
    stack[head]->unitsPerSecond = unitsPerSecond;

    head = (head + 1) % stackSize;

    return true;
}

Movement* Axis::popMove() {
    Movement* out = stack[tail];

    if (out == nullptr) {
        return nullptr;
    }

    stack[tail] = nullptr;
    tail = (tail + 1) % stackSize;

    Serial.println("POP");

    return out;
}

Movement* Axis::peekMove() {
    return stack[tail];
}

unsigned short Axis::getStackSize() {
    if (head < tail) {
        return getMaxStackSize() - tail + head;
    }

    if (head == tail && stack[tail] != nullptr) {
        return getMaxStackSize();
    }

    return head - tail;   
}

unsigned short Axis::getMaxStackSize() {
    return stackSize;
}

void Axis::setMaxStackSize(unsigned short size) {
    Movement** newStack = new Movement*[size];

    head = getStackSize();

    // Copy old stack and clean 
    for (unsigned short i = 0; i < size; i++) {
        if (i < stackSize) {
            newStack[i] = stack[(tail + i) % stackSize];
        } else {
            newStack[i] = nullptr;
        }
    }

    if (stack != nullptr) {
        delete stack;
    }

    tail = 0;

    stackSize = size;
    stack = newStack;
}