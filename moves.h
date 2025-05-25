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
} movelist_node_t;

void move_piece(square_t board[8][8], move_t move);
void highlight_legal_moves(square_t board[8][8], int x, int y);
movelist_node_t *get_all_legal_moves(square_t board[8][8]);
bool is_check(square_t board[8][8]);
int list_length(movelist_node_t *head);
bool movelist_contains(movelist_node_t *head, move_t check_move);
void copy_board(square_t board_copy[8][8], square_t board[8][8]);
