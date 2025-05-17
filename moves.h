#pragma once
#include "pieces.h"

extern int turn;

void move_piece(square_t board[8][8], int start_x, int start_y, int end_x, int end_y);
