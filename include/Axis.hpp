#ifndef _HPP_Axis
#define _HPP_Axis

typedef struct MotorNode {
    Motor* motor;
    char axis;
    MotorNode* next;
} MotorNode;

typedef struct Movement {
    Position* position;
    Milimeter unitsPerSecond;
} Movement;

// @todo rename movement manager or something
class Axis {
    unsigned short tail = 0;
    unsigned short head = 0;
    unsigned short stackSize = 64;
    Movement** stack;
    MotorNode* motors = nullptr;
    char activeAxisCount = 0;
    Motor** activeAxis = nullptr;

    Movement* popMove();
    Movement* peekMove();

    void startNextMove();
public:
    void setMotor(char axis, Motor* motor);
    bool deleteMotor(char axis);
    Motor* getMotor(char axis);
    bool hasMotor(char axis);

    void tick(unsigned long ms);
    void tick();

    // @todo make position a pointer?
    bool pushMove(Position* position, Milimeter unitsPerSecond);
    
    unsigned short getStackSize();
    unsigned short getMaxStackSize();
    void setMaxStackSize(unsigned short size);
    bool moving();
};

#endif // _HPP_Axis