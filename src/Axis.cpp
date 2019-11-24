#include "motor/Motor.hpp"
#include "Position.hpp"
#include "Axis.hpp"
#include "Arduino.h"
#define diff(x, y) (x > y ? x - y : y - x)

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
        Serial.println("Start next move");
        startNextMove();
    }
}

bool Axis::moving() {
    return activeAxisCount > 0;
}

void Axis::startNextMove() {
    Movement* lastMove = popMove();
    delete lastMove->position;
    delete lastMove;

    Movement* move = peekMove();

    if (move == nullptr) {
        activeAxisCount = 0;   

        Serial.println("Could't get next move");

        return;
    }

    Serial.println("Getting axis info");
    Serial.println(move->position->hasAxis('X') ? "Has X" : "No X");
    Serial.println(move->position->hasAxis('Y') ? "Has Y" : "No Y");
    Serial.println(move->position->hasAxis('Z') ? "Has Z" : "No Z");
    activeAxisCount = move->position->getAxisCount();
    Serial.print("Axiscount: ");
    Serial.println(activeAxisCount, 10);

    char axis[activeAxisCount];
    move->position->listAxis(axis);

    // Prepare motor lookup
    delete activeAxis;
    activeAxis = new Motor*[activeAxisCount];

    if (activeAxisCount == 0) {
        Serial.println("Nothing to move???");
        return;
    }

    Milimeter totalDistance = 0;
    Milimeter distances[activeAxisCount];

    Serial.println("Gathering distance");

    for (int i = 0; i < activeAxisCount; i++) {
        activeAxis[i] = getMotor(axis[i]);

        // Set position
        Milimeter axisPos = move->position->getAxis(axis[i]);
        activeAxis[i]->setTargetPosition(axisPos);

        distances[i] = diff(axisPos, activeAxis[i]->getPosition());
        totalDistance += pow(distances[i], 2);
    }

    totalDistance = sqrt(totalDistance);
    double time = totalDistance / move->unitsPerSecond;

    for (int i = 0; i < activeAxisCount; i++) {
        activeAxis[i]->setSpeed(distances[i] / time);

        Serial.print(distances[i], 10);
        Serial.print(" => ");
        Serial.println(distances[i]/time, 10);
    }

    Serial.print("Total distance: ");
    Serial.println(totalDistance, 10);
    Serial.print("Time: ");
    Serial.println(time, 10);
} 

void Axis::tick() {
    tick(micros());
}

bool Axis::pushMove(Position* position, Milimeter unitsPerSecond) {
    if (stack[head] != nullptr) {
        Serial.println("Move stack full!");
        return false;
    }

    stack[head] = new Movement;
    stack[head]->position = position;
    stack[head]->unitsPerSecond = unitsPerSecond;

    head = (head + 1) % stackSize;

    Serial.println("Push move");

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
    Movement* newStack[size];

    // Copy old stack and clean 
    for (unsigned short i = 0; i < size; i++) {
        if (i < stackSize) {
            newStack[i] = stack[(tail + i) % stackSize];
        } else {
            newStack[i] = nullptr;
        }
    }

    delete stack;

    stackSize = size;
    stack = newStack;
}