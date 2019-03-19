#include "radar.h"

Radar::Radar() {
    this->servoAngle = this->_servo.getAngle();
    this->sonarDistance = this->_sonar.getDistance();
    this->display = new GeeGrow_ILI9341();
    this->display->init(LED, RESET, DC, WR, CS);
    this->display->setRotation(ROTATION_UP);
    this->display->fillDisplay(BG_COLOR);
}

void Radar::drawNet() {
    this->display->drawCircle(159, 200, 160, NET_COLOR);
    this->display->drawLine(0, 200, 319, 200, NET_COLOR);
}

void Radar::drawSegment(int16_t xt, int16_t yt, int16_t radius, int16_t angle) {
    /* Angles */
    float angle_rad = (float)(angle) / 180.0 * M_PI;
    float delta_rad = SONAR_ACC/6;

    /* X-Coords */
    int16_t x0 = ((int16_t)((float)radius*cos(angle_rad - 3*delta_rad)));
    int16_t x1 = ((int16_t)((float)radius*cos(angle_rad - 2*delta_rad)));
    int16_t x2 = ((int16_t)((float)radius*cos(angle_rad - 1*delta_rad)));
    int16_t x3 = ((int16_t)((float)radius*cos(angle_rad)));
    int16_t x4 = ((int16_t)((float)radius*cos(angle_rad + 1*delta_rad)));
    int16_t x5 = ((int16_t)((float)radius*cos(angle_rad + 2*delta_rad)));
    int16_t x6 = ((int16_t)((float)radius*cos(angle_rad + 3*delta_rad)));
    /* Y-Coords */
    int16_t y0 = ((int16_t)((float)radius*sin(angle_rad - 3*delta_rad)));
    int16_t y1 = ((int16_t)((float)radius*sin(angle_rad - 2*delta_rad)));
    int16_t y2 = ((int16_t)((float)radius*sin(angle_rad - 1*delta_rad)));
    int16_t y3 = ((int16_t)((float)radius*sin(angle_rad)));
    int16_t y4 = ((int16_t)((float)radius*sin(angle_rad + 1*delta_rad)));
    int16_t y5 = ((int16_t)((float)radius*sin(angle_rad + 2*delta_rad)));
    int16_t y6 = ((int16_t)((float)radius*sin(angle_rad + 3*delta_rad)));

    this->display->drawLine(xt - x0, yt - y0, xt - x1, yt - y1, NET_COLOR);
    this->display->drawLine(xt - x1, yt - y1, xt - x2, yt - y2, NET_COLOR);
    this->display->drawLine(xt - x2, yt - y2, xt - x3, yt - y3, NET_COLOR);
    this->display->drawLine(xt - x3, yt - y3, xt - x4, yt - y4, NET_COLOR);
    this->display->drawLine(xt - x4, yt - y4, xt - x5, yt - y5, NET_COLOR);
    this->display->drawLine(xt - x5, yt - y5, xt - x6, yt - y6, NET_COLOR);
}