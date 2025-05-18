#pragma once
#include "pieces.h"

extern int turn;

typedef struct Move {
    int start_x;
    int start_y;
    int end_x;
    int end_y;
} move_t;

typedef struct Node {
    move_t move;
    struct Node *next;
} node_t;

void push_end(node_t *head, node_t *move_node);
void move_piece(square_t board[8][8], move_t move);

void _highlight_rook_moves(node_t *possible_moves, square_t board[8][8], int x, int y);
