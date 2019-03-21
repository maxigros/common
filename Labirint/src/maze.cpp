#include "maze.h"

Maze::Maze() {
    /* Init display */
    Drawer::instance().init(LED, RESET, DC, WR, CS);
    Drawer::instance().setRotation(ROTATION_UP);
    Drawer::instance().fillDisplay(BLACK);

    /* Prepare for maze creation */
    randomSeed(analogRead(11));
    this->stack.cells = malloc(12*16 * sizeof(cell_t));
    this->stack.pos = 0;
    this->neighbours.cells = malloc(4 * sizeof(cell_t));
    this->neighbours.size = 0;

    /* Generate maze */
    this->generate();

    /* Garbage after generation */
    free(this->stack.cells);
    free(this->neighbours.cells);

    /* Print maze on display */
    this->printMaze();

    /* Create ball */
    this->ball = new Ball();
    this->ball->setBrickSize(this->brick_size);
}

void Maze::upd(uint8_t direction){
    switch (direction) {
        case LEFT:
            this->moveBallLeft();
            break;
        case RIGHT:
            this->moveBallRight();
            break;
        case UP:
            this->moveBallUp();
            break;
        case DOWN:
            this->moveBallDown();
            break;
        default:
            break;
    }
}

void Maze::moveBallLeft() {
    int16_t current_x = this->ball->getMazeX();
    int16_t current_y = this->ball->getMazeY();
    /* Check if possible */
    if (this->map[current_x - 1][current_y] == CELL_VISITED)
        this->ball->moveLeftMaze();
}

void Maze::moveBallRight() {
    int16_t current_x = this->ball->getMazeX();
    int16_t current_y = this->ball->getMazeY();

    /* Check if possible */
    if (this->map[current_x + 1][current_y] == CELL_VISITED)
        this->ball->moveRightMaze();

    /* Finish */
    if (this->ball->getMazeX() == 30) {
        Drawer::instance().fillDisplay(BLACK);
    }
}

void Maze::moveBallUp() {
    int16_t current_x = this->ball->getMazeX();
    int16_t current_y = this->ball->getMazeY();
    /* Check if possible */
    if (this->map[current_x][current_y - 1] == CELL_VISITED)
        this->ball->moveUpMaze();
}

void Maze::moveBallDown() {
    int16_t current_x = this->ball->getMazeX();
    int16_t current_y = this->ball->getMazeY();
    /* Check if possible */
    if (this->map[current_x][current_y + 1] == CELL_VISITED)
        this->ball->moveDownMaze();
}

void Maze::printMaze() {
    for (uint16_t j = 0; j < HEIGHT; j++) {
        for (uint16_t i = 0; i < WIDTH; i++) {
            if (this->map[i][j] == WALL) {
                Drawer::instance().fillRect(
                    i*this->brick_size, 
                    j*this->brick_size, 
                    this->brick_size, 
                    this->brick_size, 
                    this->mazeColor
                );
            }
            if (this->map[i][j] == CELL_VISITED) {
                Drawer::instance().fillRect(
                    i*this->brick_size, 
                    j*this->brick_size, 
                    this->brick_size, 
                    this->brick_size, 
                    this->mazeBgColor
                );
            }
        }
    }
    /* Entrance */
    Drawer::instance().fillRect(
        0*this->brick_size, 
        21*this->brick_size, 
        this->brick_size, 
        this->brick_size, 
        this->mazeBgColor
    );
    /* Exit */
    Drawer::instance().fillRect(
        30*this->brick_size, 
        1*this->brick_size, 
        this->brick_size, 
        this->brick_size, 
        this->mazeBgColor
    );
}

void Maze::serialMaze() {
    for (uint16_t j = 0; j < HEIGHT; j++) {
        for (uint16_t i = 0; i < WIDTH; i++) {
            Serial.print(this->map[i][j]); 
            Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println(); Serial.println();
}

void Maze::generate() {
    /* First init */
    for (uint16_t j = 1; j < HEIGHT; j+=2){
        for (uint16_t i = 1; i < WIDTH; i+=2){
            this->map[i][j] = CELL_UNVISITED;
        }
    }

    cell_t cellCurrent = {1, 1};
    this->map[cellCurrent.x][cellCurrent.y] = CELL_VISITED;
    do {
        this->getNewNeighbours(cellCurrent);
        if (this->neighbours.size != 0) {
            uint8_t direction = randRange(this->neighbours.size);
            cell_t cellNeighbour = this->neighbours.cells[direction];
            stack_push(cellCurrent);
            this->removeWall(cellCurrent, cellNeighbour);
            cellCurrent = cellNeighbour;
            this->map[cellCurrent.x][cellCurrent.y] = CELL_VISITED;
        } else {
            cellCurrent = this->stack_pop();
        }
    } while(this->getUnvisitedNum() > 0);

    for (uint16_t j = 0; j < HEIGHT; j++) {
        for (uint16_t i = 0; i < WIDTH; i++) {
            uint8_t temp = this->map[i][j];
            if (temp == EX_WALL)
                this->map[i][j] = CELL_VISITED;
        }
    }

    this->map[30][1] = CELL_VISITED;
}

uint8_t Maze::randRange(uint8_t range) {
    return random() % range;
}

void Maze::getNewNeighbours(cell_t c) {
    cell_t possible_neighbours[4];
    possible_neighbours[0] = {c.x, c.y - this->step};    /* down */
    possible_neighbours[1] = {c.x + this->step, c.y};    /* right */
    possible_neighbours[2] = {c.x, c.y + this->step};    /* up */
    possible_neighbours[3] = {c.x - this->step, c.y};    /* left */

    uint8_t resBoxSize = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (   possible_neighbours[i].x > 0 
            && possible_neighbours[i].x < WIDTH 
            && possible_neighbours[i].y > 0 
            && possible_neighbours[i].y < HEIGHT
        ){
            uint8_t temp = map[possible_neighbours[i].x][possible_neighbours[i].y];
            if(temp == CELL_UNVISITED){
                this->neighbours.cells[resBoxSize] = possible_neighbours[i];
                resBoxSize++;
            }
        }
    }
    this->neighbours.size = resBoxSize;
}

uint16_t Maze::getUnvisitedNum() {
    uint16_t res = 0;
    for (uint16_t j = 0; j < HEIGHT; j++)
        for (uint16_t i = 0; i < WIDTH; i++)
            if (this->map[i][j] == CELL_UNVISITED)
                res++;

    return res;
}

void Maze::removeWall(cell_t c1, cell_t c2) {
    int16_t diff_x = c2.x - c1.x;
    int16_t diff_y = c2.y - c1.y;
    cell_t cellRes;

    if (diff_x != 0) {
        cellRes.y = c1.y;
        cellRes.x = (diff_x > 0) ? (c1.x + 1) : (c2.x + 1);
    } else if (diff_y != 0) {
        cellRes.x = c1.x;
        cellRes.y = (diff_y > 0) ? (c1.y + 1) : (c2.y + 1);
    }

    this->map[cellRes.x][cellRes.y] = EX_WALL;
}

void Maze::stack_push(cell_t c) {
    this->stack.cells[this->stack.pos] = c;
    this->stack.pos++;
}

cell_t Maze::stack_pop() {
    this->stack.pos--;
    return this->stack.cells[this->stack.pos];
}
