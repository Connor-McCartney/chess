#include "pieces.h"
#include <assert.h>

int turn = WHITE;

void move_piece(square_t board[8][8], int start_x, int start_y, int end_x, int end_y) {
    assert(start_x >= 0);
    assert(start_y >= 0);
    assert(end_x >= 0);
    assert(end_y >= 0);
    assert(start_x <= 7);
    assert(start_y <= 7);
    assert(end_x <= 7);
    assert(end_y <= 7);

    // en passant and castling are exceptions...

    // piece at end becomes piece at start, and no longer highlighted
    board[end_x][end_y].piece = board[start_x][start_y].piece;
    board[end_x][end_y].highlight = NORMAL;

    // piece at start becomes empty
    board[start_x][start_y].piece = empty;
}
