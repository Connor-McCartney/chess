enum Colours {
    WHITE, 
    BLACK,
};

enum Pieces {
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PAWN,
};

typedef struct Piece {
    char *unicode_char;
    int colour;
    int piece;
} piece_t;


struct Piece white_rook = {"♜", WHITE, ROOK};
struct Piece white_knight = {"♞", WHITE, KNIGHT};
struct Piece white_bishop = {"♝", WHITE, BISHOP};
struct Piece white_queen = {"♛", WHITE, QUEEN};
struct Piece white_king = {"♚", WHITE, KING};
struct Piece white_pawn = {"♟", WHITE, PAWN};

struct Piece black_rook = {"♖", BLACK, ROOK};
struct Piece black_knight = {"♘", BLACK, KNIGHT};
struct Piece black_bishop = {"♗", BLACK, BISHOP};
struct Piece black_queen = {"♕", BLACK, QUEEN};
struct Piece black_king = {"♔", BLACK, KING};
struct Piece black_pawn = {"♙", BLACK, PAWN};

struct Piece empty = {"", -1, -1};

void initalise_board(piece_t *board[8][8]) {
    for (int i=0; i<8; i++) {
        board[i][1] = &white_pawn;
        board[i][6] = &black_pawn;
    }

    for (int i=0; i<8; i++) {
        for (int j=2; j<6; j++) {
            board[i][j] = &empty;
        }
    }

    board[0][0] = &white_rook;
    board[1][0] = &white_knight;
    board[2][0] = &white_bishop;
    board[3][0] = &white_queen;
    board[4][0] = &white_king;
    board[5][0] = &white_bishop;
    board[6][0] = &white_knight;
    board[7][0] = &white_rook;

    board[0][7] = &black_rook;
    board[1][7] = &black_knight;
    board[2][7] = &black_bishop;
    board[3][7] = &black_queen;
    board[4][7] = &black_king;
    board[5][7] = &black_bishop;
    board[6][7] = &black_knight;
    board[7][7] = &black_rook;
}
