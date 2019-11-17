#ifndef _HPP_MOTOR
#define _HPP_MOTOR

typedef float Milimeter;

class Motor {
public:
    virtual Milimeter getPosition();
    virtual Milimeter getTargetPosition();
    virtual void setTargetPosition(Milimeter position, bool relative = false);
    virtual Milimeter getSpeed();
    virtual void setSpeed(Milimeter speed);

    virtual void tick(unsigned long ms);

    virtual void home();
    virtual bool homed();
    virtual void disable();
    virtual void enable();
    virtual bool enabled();
};

#endif // _HPP_MOTOR
