#include "motor/Motor.hpp"
#include "Position.hpp"

void Position::setAxis(char axis, Milimeter position) {
    PositionNode* curr = positions;

    while (curr) {
        if (curr->axis == axis) {
            curr->position = position;

            return;
        }

        curr = curr->next;
    }

    PositionNode* newPos = new PositionNode;
    newPos->axis = axis;
    newPos->position = position;
    newPos->next = positions;

    positions = newPos;
}

Milimeter Position::getAxis(char axis) {
    PositionNode* curr = positions;

    while (curr) {
        if (curr->axis == axis) {
            return curr->position;
        }

        curr = curr->next;
    }

    return 0;
}

bool Position::delAxis(char axis){
    PositionNode* curr = positions;

    if (positions->axis == axis) {
        positions = curr->next;

        delete curr;
        return true;
    }

    PositionNode* last;

    while ((last = curr) && (curr = curr->next)) {
        if (curr->axis == axis) {
            last->next = curr->next;

            delete curr;
            return true;
        }
    }

    return false;
}


bool Position::hasAxis(char axis){
    PositionNode* last = this->positions;

    while (last) {
        if (last->axis == axis) {
            return true;
        }

        last = last->next;
    }

    return false;
}

char Position::getAxisCount(){
    char count = 0;

    PositionNode* last = this->positions;

    while (last) {
        count++;
        last = last->next;
    }

    return count;
}

void Position::listAxis(char* output) {
    char index = 0;
    PositionNode* last = this->positions;

    while (last) {
        output[index] = last->axis;
        last = last->next;

        index++;
    }
}