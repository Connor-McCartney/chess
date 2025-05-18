#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stddef.h>
#include "pieces.h"
#include "moves.h"


void push_end(node_t *head, node_t *move_node) {
    node_t *current = head;
    /*
    while (current->next != NULL) {
        current = current->next;
    }
    */
    current->next = move_node;
}

int turn = WHITE;

void move_piece(square_t board[8][8], move_t move) {
    assert(move.start_x >= 0);
    assert(move.start_y >= 0);
    assert(move.end_x >= 0);
    assert(move.end_y >= 0);
    assert(move.start_x <= 7);
    assert(move.start_y <= 7);
    assert(move.end_x <= 7);
    assert(move.end_y <= 7);

    // en passant, promotion, and castling are exceptions...

    // piece at end becomes piece at start, and no longer highlighted
    board[move.end_x][move.end_y].piece = board[move.start_x][move.start_y].piece;
    board[move.end_x][move.end_y].highlight = NORMAL;

    // piece at start becomes empty
    board[move.start_x][move.start_y].piece = empty;
}


void _highlight_rook_moves(node_t *possible_moves, square_t board[8][8], int x, int y) {
    assert(x >= 0);
    assert(x >= 0);
    assert(y <= 7);
    assert(x <= 7);
    assert(board[x][y].piece.piece == ROOK);
    assert(board[x][y].piece.colour != EMPTY);
    int col = board[x][y].piece.colour;
    int i;



    // up
    i = y;
    while (true) {
        i += col;
        if (i > 7 || i < 0) {
            break;
        }
        if (board[x][i].piece.colour != col) {
            move_t move = {x, y, x, i};
            node_t move_node = {move, NULL};
            push_end(possible_moves, &move_node);
        }
        if (board[x][i].piece.colour != EMPTY) {
            break;
        } 


        node_t *current = possible_moves;
        while (current->next != NULL) {
            current = current->next;
            int end_x = current->move.end_x;
            int end_y = current->move.end_y;
            if (board[end_x][end_y].piece.colour == EMPTY) {
                board[end_x][end_y].piece = dot;
            }
            if (board[x][i].piece.colour != col) {
                board[x][i].highlight = LEGAL;
            }
        }
    }
}
