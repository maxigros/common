#ifndef SERVO_H
#define SERVO_H

#define SERVO_PWM    4

#include <Arduino.h>

class Servo {
    public:
        Servo();
        uint16_t getAngle();
};

#endif /* SERVO_H */