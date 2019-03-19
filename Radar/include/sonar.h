#ifndef SONAR_H
#define SONAR_H

#define TRIG    8
#define ECHO    9

#define SONAR_ACC   0.26F  /* Rads */

#include <Arduino.h>

class Sonar {
    public:
        Sonar();
        uint16_t getDistance();
};

#endif /* SONAR_H */