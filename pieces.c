#include "pieces.h"

piece_t white_rook = {"♜", WHITE, ROOK};
piece_t white_knight = {"♞", WHITE, KNIGHT};
piece_t white_bishop = {"♝", WHITE, BISHOP};
piece_t white_queen = {"♛", WHITE, QUEEN};
piece_t white_king = {"♚", WHITE, KING};
piece_t white_pawn = {"♟", WHITE, PAWN};

piece_t black_rook = {"♖", BLACK, ROOK};
piece_t black_knight = {"♘", BLACK, KNIGHT};
piece_t black_bishop = {"♗", BLACK, BISHOP};
piece_t black_queen = {"♕", BLACK, QUEEN};
piece_t black_king = {"♔", BLACK, KING};
piece_t black_pawn = {"♙", BLACK, PAWN};

piece_t empty = {" ", -4, -4};

void initalise_board(square_t board[8][8]) {
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            board[i][j].piece = empty;
        }
    }


    /*
    for (int i=0; i<8; i++) {
        board[i][1].piece = white_pawn;
        board[i][6].piece = black_pawn;
    }
    */



    board[0][0].piece = white_rook;
    board[1][0].piece = white_knight;
    board[2][0].piece = white_bishop;
    board[3][0].piece = white_queen;
    board[4][0].piece = white_king;
    board[5][0].piece = white_bishop;
    board[6][0].piece = white_knight;
    board[7][0].piece = white_rook;

    board[0][7].piece = black_rook;
    board[1][7].piece = black_knight;
    board[2][7].piece = black_bishop;
    board[3][7].piece = black_queen;
    board[4][7].piece = black_king;
    board[5][7].piece = black_bishop;
    board[6][7].piece = black_knight;
    board[7][7].piece = black_rook;
}
