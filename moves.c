#include <assert.h>
#include "pieces.h"
#include "moves.h"

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
