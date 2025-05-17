#pragma once

enum Colours {
    WHITE = 1, 
    EMPTY = 0,
    BLACK = -1,
};

enum Pieces {
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PAWN,
    DOT,
    BLANK,
};

enum Highlights {
    NORMAL,
    CLICKED,
    LEGAL,
};

typedef struct Piece {
    char *unicode_char;
    int colour;
    int piece;
} piece_t;


typedef struct Square{
    piece_t piece;
    int x;
    int y;
    int highlight;
} square_t;


extern piece_t white_rook;
extern piece_t white_knight;
extern piece_t white_bishop; 
extern piece_t white_queen; 
extern piece_t white_king; 
extern piece_t white_pawn; 
extern piece_t black_rook; 
extern piece_t black_knight; 
extern piece_t black_bishop; 
extern piece_t black_queen; 
extern piece_t black_king; 
extern piece_t black_pawn; 
extern piece_t empty; 
extern piece_t dot; 

void initalise_board(square_t board[8][8]);
