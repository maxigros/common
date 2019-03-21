#ifndef MAZE_H
#define MAZE_H

#include "ball.h"

/* Pinout display*/
#define LED   A0
#define RESET A3
#define DC    A1
#define WR    A4
#define CS    A2

/* Maze properties */
#define WIDTH      31
#define HEIGHT     23

/* Directions */
enum {
    LEFT = 1,
    RIGHT,
    UP,
    DOWN
};

/* Cells' values */
enum {
    WALL,
    CELL_UNVISITED,
    CELL_VISITED,
    EX_WALL
};

typedef struct {
    int16_t x;
    int8_t y;
} cell_t;

typedef struct {
    cell_t* cells;
    uint16_t size;
} cellBox_t;

typedef struct {
    cell_t* cells;
    uint16_t pos;
} stack_t;

class Maze {
    public:
        Maze();
        void upd(uint8_t direction);

        uint8_t map[WIDTH][HEIGHT] = {{WALL}};

    private:
        void generate();
        void getNewNeighbours(cell_t c);
        void removeWall(cell_t c1, cell_t c2);
        uint16_t getUnvisitedNum();
        void stack_push(cell_t c);
        cell_t stack_pop();
        uint8_t randRange(uint8_t range);
        void serialMaze();
        void printMaze();
        void moveBallLeft();
        void moveBallRight();
        void moveBallUp();
        void moveBallDown();

        Ball* ball;
        cellBox_t neighbours;
        stack_t stack;
        const uint8_t step = 2;
        const uint8_t brick_size = 320 / (WIDTH + 1);
        const uint16_t mazeColor = RED;
        const uint16_t mazeBgColor = WHITE;
};


#endif /* MAZE_H */