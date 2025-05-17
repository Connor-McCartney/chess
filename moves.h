#pragma once
#include "pieces.h"

extern int turn;

typedef struct Move {
    int start_x;
    int start_y;
    int end_x;
    int end_y;
} move_t;

void move_piece(square_t board[8][8], move_t move);
