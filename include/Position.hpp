#ifndef __HPP_POSITION
#define __HPP_POSITION

typedef struct PositionNode {
    char axis;
    Milimeter position; 
    struct PositionNode* next = nullptr;
} PositionNode;

class Position {
    PositionNode* positions = nullptr;

public:
    ~Position();
    void setAxis(char axis, Milimeter position);
    Milimeter getAxis(char axis);
    bool delAxis(char axis);
    bool hasAxis(char axis);

    char getAxisCount();
    void listAxis(char* output);
};

#endif // __HPP_POSITION