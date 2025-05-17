#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "pieces.h"
#include "moves.h"
#include "linked_list.h"


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


void _highlight_rook_moves(square_t board[8][8], int x, int y) {
    assert(x >= 0);
    assert(x >= 0);
    assert(y <= 7);
    assert(x <= 7);
    assert(board[x][y].piece.piece == ROOK);
    assert(board[x][y].piece.colour != EMPTY);
    int col = board[x][y].piece.colour;
    int i;

    node_t moves = {{-1, -1, -1, -1}, NULL}; 

    // up
    i = y;
    while (true) {
        i += col;
        if (i > 7 || i < 0) {
            break;
        }
        if (board[x][i].piece.colour != col) {
            board[x][i].highlight = LEGAL;
            move_t move = {x, y, x, i};
            append_move(moves, move);
        }
        if (board[x][i].piece.colour == EMPTY) {
            board[x][i].piece = dot;
        } else {
            break;
        }
    }
}
