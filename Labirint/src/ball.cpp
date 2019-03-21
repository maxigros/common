#include "ball.h"

Ball::Ball() {
    #ifdef FILLED
        this->circle_obj = Drawer::instance().createFillCircleObj();
    #else
        this->circle_obj = Drawer::instance().createCircleObj();
    #endif /* FILLED */
    this->circle_obj->setColorBG(this->bg_color);
    /* Ball to entrance */
    this->setNull();
}

void Ball::setNull() {
    this->maze_x = 0;
    this->maze_y = 21;
    this->circle_obj->moveTo(5, 215, 4, this->color);
}

int16_t Ball::getMazeX() {
    return this->maze_x;
}

int16_t Ball::getMazeY() {
    return this->maze_y;
}

void Ball::setBrickSize(uint8_t size) {
    this->brickSize = size;
    this->circle_obj->setRadius(this->brickSize/2 - 1);
}

void Ball::moveLeftMaze() {
    this->maze_x--;
    for (uint8_t i = 0; i < this->brickSize/5; i++) {
        this->circle_obj->moveLeft(5);
        delay(DELAY_MS);
    }
}

void Ball::moveRightMaze() {
    this->maze_x++;
    for (uint16_t i = 0; i < this->brickSize/5; i++) {
        this->circle_obj->moveRight(5);
        delay(DELAY_MS);
    }
}

void Ball::moveUpMaze() {
    this->maze_y--;
    for (uint8_t i = 0; i < this->brickSize/5; i++) {
        this->circle_obj->moveUp(5);
        delay(DELAY_MS);
    }
}

void Ball::moveDownMaze() {
    this->maze_y++;
    for (uint8_t i = 0; i < this->brickSize/5; i++) {
        this->circle_obj->moveDown(5);
        delay(DELAY_MS);
    }
}