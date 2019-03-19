#ifndef RADAR_H
#define RADAR_H

#define LED    A4
#define RESET  A3
#define DC     A2
#define WR     A1
#define CS     A0

#define BG_COLOR    WHITE
#define NET_COLOR   BLACK

#include <GeeGrow_ILI9341.h>
#include <math.h>
#include "sonar.h"
#include "servo.h"

class Radar {
    public:
        Radar();
        void drawNet();
        void drawSegment(int16_t x0, int16_t y0, int16_t radius, int16_t angle);

    private:
        uint16_t sonarDistance;
        uint16_t servoAngle;
        Sonar _sonar;
        Servo _servo;
        GeeGrow_ILI9341* display;

};

#endif /* RADAR_H */