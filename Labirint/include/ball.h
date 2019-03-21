#ifndef BALL_H
#define BALL_H

#define FILLED
#define DELAY_MS    5

#include "drawer.h"

class Ball {
    public:
        Ball();
        void setBrickSize(uint8_t size);
        int16_t getMazeX();
        int16_t getMazeY();
        void moveLeftMaze();
        void moveRightMaze();
        void moveUpMaze();
        void moveDownMaze();
        void setNull();


    private:
        #ifdef FILLED
            GeeGrow_ILI9341_objectFillCircle* circle_obj = nullptr;
        #else
            GeeGrow_ILI9341_objectCircle* circle_obj = nullptr;
        #endif /* FILLED */
        int16_t maze_x;
        int16_t maze_y;
        uint8_t brickSize = 10;
        const uint16_t color = BLACK;
        const uint16_t bg_color = WHITE;
};

#endif /* BALL_H */