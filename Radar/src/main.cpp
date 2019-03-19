#include "radar.h"

Radar* _radar;

void setup() {
    // Serial.begin(9600);
    // while(!Serial);

    _radar = new Radar();
    // _radar->drawNet();
    _radar->drawSegment(159, 200, 160, 90);

}

void loop() {

}