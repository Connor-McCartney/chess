#pragma once
#include "moves.h"

typedef struct Node {
    move_t move;
    struct Node *next;
} node_t;


void append_move(node_t current, move_t move);
