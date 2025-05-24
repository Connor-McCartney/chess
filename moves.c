#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "pieces.h"
#include "moves.h"


int turn = WHITE;

movelist_node_t *create_movelist_node(move_t move){
    movelist_node_t *new_node = malloc(sizeof(movelist_node_t));
    new_node->move = move;
    new_node->next = NULL;
    return new_node;
}

void destroy_list(movelist_node_t *head){
    movelist_node_t *next;
    for (movelist_node_t *current = head;  current != NULL;  current = next) {
        next = current->next;
        free(current);
    }
}

int list_length(movelist_node_t *head){
    int length = 0;
    for (movelist_node_t *current = head; current != NULL; current = current->next) {
        length += 1;
    }
    return length;
}

bool movelist_contains(movelist_node_t *head, move_t check_move){
    for (movelist_node_t *current = head; current != NULL; current = current->next) {
        move_t move = current-> move;
        if (move.start_x == check_move.start_x && \
                move.start_y == check_move.start_y && \
                move.end_x   == check_move.end_x   && \
                move.end_y   == check_move.end_y) {
            return true;
        }
    }
    return false;
}

movelist_node_t *movelist_push_end(movelist_node_t *head, move_t move) {
    movelist_node_t *new_node = create_movelist_node(move);
    movelist_node_t *current;

    if (head==NULL) {
        head = new_node;
    } else {
        for (current = head; current->next != NULL; current = current->next) {
            ;
        }
        current->next = new_node;
    }
    return head;
}

void move_piece(square_t board[8][8], move_t move) {
    piece_t start = board[move.start_x][move.start_y].piece;

    // en passant, promotion, and castling are exceptions...

    // I'll just auto-queen instead of making an underpromotion popup
    if (start.piece == PAWN && start.colour == WHITE && move.end_y == 7) {
        board[move.end_x][move.end_y].piece = white_queen;
        board[move.start_x][move.start_y].piece = empty;
        return;
    }
    if (start.piece == PAWN && start.colour == BLACK && move.end_y == 0) {
        board[move.end_x][move.end_y].piece = black_queen;
        board[move.start_x][move.start_y].piece = empty;
        return;
    }

    // regular moves

    // piece at end becomes piece at start
    board[move.end_x][move.end_y].piece = start;
    // piece at start becomes empty
    board[move.start_x][move.start_y].piece = empty;
}


movelist_node_t *get_possible_rook_moves(square_t board[8][8], int x, int y) {
    int col = board[x][y].piece.colour;
    int xx, yy;

    movelist_node_t *possible_moves = NULL;

    int up_down[4] = {0, 0, -1, 1};
    int left_right[4] = {-1, 1, 0, 0};
    for (int i=0; i<4; i++) {
        yy = y;
        xx = x;
        while (true) {
            xx += left_right[i];
            yy += up_down[i];
            if (xx > 7 || yy > 7 || xx < 0 || yy < 0) {
                break;
            }
            if (board[xx][yy].piece.colour != col) {
                move_t move = {x, y, xx, yy};
                possible_moves = movelist_push_end(possible_moves, move);
            }
            if (board[xx][yy].piece.colour != EMPTY) {
                break;
            } 
        }
    }

    return possible_moves;
}

movelist_node_t *get_possible_knight_moves(square_t board[8][8], int x, int y) {
    int col = board[x][y].piece.colour;
    int xx, yy;

    movelist_node_t *possible_moves = NULL;


    for (int i=-2; i<=2; i++) {
        for (int j=-2; j<=2; j++) {
            if (i==0 || j==0 || abs(i) == abs(j)) {
                continue;
            }
            yy = y + i;
            xx = x + j;
            if (xx > 7 || yy > 7 || xx < 0 || yy < 0) {
                continue;
            }
            if (board[xx][yy].piece.colour != col) {
                move_t move = {x, y, xx, yy};
                possible_moves = movelist_push_end(possible_moves, move);
            }
            if (board[xx][yy].piece.colour != EMPTY) {
                continue;
            } 
        }
    }

    return possible_moves;
}

movelist_node_t *get_possible_king_moves(square_t board[8][8], int x, int y) {
    int col = board[x][y].piece.colour;
    int xx, yy;

    movelist_node_t *possible_moves = NULL;

    for (int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            yy = y + i;
            xx = x + j;
            if (xx > 7 || yy > 7 || xx < 0 || yy < 0) {
                continue;
            }
            if (board[xx][yy].piece.colour != col) {
                move_t move = {x, y, xx, yy};
                possible_moves = movelist_push_end(possible_moves, move);
            }
            if (board[xx][yy].piece.colour != EMPTY) {
                continue;
            } 
        }
    }

    return possible_moves;
}


movelist_node_t *get_possible_bishop_moves(square_t board[8][8], int x, int y) {
    int col = board[x][y].piece.colour;
    int xx, yy;

    movelist_node_t *possible_moves = NULL; 


    for (int i=-1; i<=1; i+=2) {
        for (int j=-1; j<=1; j+=2) {
            yy = y;
            xx = x;
            while (true) {
                xx += i;
                yy += j;
                if (xx > 7 || yy > 7 || xx < 0 || yy < 0) {
                    break;
                }
                if (board[xx][yy].piece.colour != col) {
                    move_t move = {x, y, xx, yy};
                    possible_moves = movelist_push_end(possible_moves, move);
                }
                if (board[xx][yy].piece.colour != EMPTY) {
                    break;
                } 
            }
        }
    }


    return possible_moves;
}

movelist_node_t *get_possible_queen_moves(square_t board[8][8], int x, int y) {
    int col = board[x][y].piece.colour;
    int xx, yy;

    movelist_node_t *possible_moves = NULL;


    for (int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            yy = y;
            xx = x;
            while (true) {
                xx += i;
                yy += j;
                if (xx > 7 || yy > 7 || xx < 0 || yy < 0) {
                    break;
                }
                if (board[xx][yy].piece.colour != col) {
                    move_t move = {x, y, xx, yy};
                    possible_moves = movelist_push_end(possible_moves, move);
                }
                if (board[xx][yy].piece.colour != EMPTY) {
                    break;
                } 
            }
        }
    }

    return possible_moves;
}

movelist_node_t *get_possible_pawn_moves(square_t board[8][8], int x, int y) {
    int col = board[x][y].piece.colour;
    int xx, yy;

    movelist_node_t *possible_moves = NULL;

    if ((col == WHITE && y == 1) || (col == BLACK && y == 6)) {
        // forward 1 or 2 squares
        yy = y;
        for (int t=0; t<2; t++) {
            yy += col;
            if (board[x][yy].piece.colour == EMPTY) {
                move_t move = {x, y, x, yy};
                possible_moves = movelist_push_end(possible_moves, move);
            } else {
                break;
            } 
        }

    } else {
        // forward only 1 square
        yy = y;
        yy += col;
        if (yy <= 7 && yy >= 0) {
            if (board[x][yy].piece.colour == EMPTY) {
                move_t move = {x, y, x, yy};
                possible_moves = movelist_push_end(possible_moves, move);
            }
        }
    }

    // captures
    for (int i=-1; i<=1; i+=2) {
        xx = x + i;
        yy = y + col;
        if (xx <= 7 && yy <= 7 && xx >= 0 && yy >= 0) {
            if (board[xx][yy].piece.colour == -col) {
                move_t move = {x, y, xx, yy};
                possible_moves = movelist_push_end(possible_moves, move);
            }
        }
    }

    return possible_moves;
}

void highlight_moves(square_t board[8][8], movelist_node_t *legal_moves) {
    for (movelist_node_t *current = legal_moves; current != NULL; current = current->next) {
        move_t move = current->move;
        int end_x = move.end_x;
        int end_y = move.end_y;
        board[end_x][end_y].highlight = LEGAL;
        if (board[end_x][end_y].piece.colour == EMPTY) {
            board[end_x][end_y].piece = dot;
        }
    }
}


movelist_node_t *get_piece_possible_moves(square_t board[8][8], int x, int y) {
    piece_t piece = board[x][y].piece;
    movelist_node_t *possible_moves = NULL;

    switch (piece.piece) {
        case ROOK:
            possible_moves = get_possible_rook_moves(board, x, y);
            break;
        case KING:
            possible_moves = get_possible_king_moves(board, x, y);
            break;
        case QUEEN:
            possible_moves = get_possible_queen_moves(board, x, y);
            break;
        case PAWN:
            possible_moves = get_possible_pawn_moves(board, x, y);
            break;
        case BISHOP:
            possible_moves = get_possible_bishop_moves(board, x, y);
            break;
        case KNIGHT:
            possible_moves = get_possible_knight_moves(board, x, y);
            break;
    }
    return possible_moves;
}

movelist_node_t *get_piece_legal_moves(square_t board[8][8], int x, int y) {
    movelist_node_t *possible_moves = get_piece_possible_moves(board, x, y);
    movelist_node_t *legal_moves = NULL;


    for (movelist_node_t *current = possible_moves; current != NULL; current = current->next) {
        move_t move = current->move;

        square_t board_copy[8][8];
        for (int i=0; i<8; i++) {
            for (int j=0; j<8; j++) {
                board_copy[i][j] = board[i][j];
            }
        }

        // play the move, and see if we are check
        move_piece(board_copy, move);
        if (is_check(board_copy) == false) {
            legal_moves = movelist_push_end(legal_moves, move);
        }
    }

    destroy_list(possible_moves);
    return legal_moves;
}

void highlight_legal_moves(square_t board[8][8], int x, int y) {
    piece_t piece = board[x][y].piece;
    if (piece.colour != turn) {
        return;
    }
    movelist_node_t *legal_moves = get_piece_legal_moves(board, x, y);
    highlight_moves(board, legal_moves);
    destroy_list(legal_moves);
}

movelist_node_t *get_all_possible_moves(square_t board[8][8]) {
    movelist_node_t *all_possible_moves = NULL;

    for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++) {
            piece_t piece = board[x][y].piece;

            if (piece.colour != turn) {
                continue;
            }

            movelist_node_t *possible_moves = get_piece_possible_moves(board, x, y);
            for (movelist_node_t *current = possible_moves; current != NULL; current = current->next) {
                move_t move_copy = {current->move.start_x, current->move.start_y, current->move.end_x, current->move.end_y};
                all_possible_moves = movelist_push_end(all_possible_moves, move_copy);
            }
            destroy_list(possible_moves);
        }
    }


    assert (all_possible_moves != NULL);
    return all_possible_moves;
}


bool is_check(square_t board[8][8]) {
    turn *= -1;
    movelist_node_t *legal_moves = get_all_possible_moves(board);
    turn *= -1;
    for (movelist_node_t *current = legal_moves; current != NULL; current = current->next) {
        int end_x = current->move.end_x;
        int end_y = current->move.end_y;
        if (board[end_x][end_y].piece.piece == KING && board[end_x][end_y].piece.colour == turn) {
            return true;
        }
    }
    return false;
}

movelist_node_t *get_all_legal_moves(square_t board[8][8]) {
    movelist_node_t *all_possible_moves = NULL;

    for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++) {
            piece_t piece = board[x][y].piece;

            if (piece.colour != turn) {
                continue;
            }

            movelist_node_t *possible_moves = get_piece_legal_moves(board, x, y);
            for (movelist_node_t *current = possible_moves; current != NULL; current = current->next) {
                move_t move_copy = {current->move.start_x, current->move.start_y, current->move.end_x, current->move.end_y};
                all_possible_moves = movelist_push_end(all_possible_moves, move_copy);
            }
            destroy_list(possible_moves);
        }
    }
    return all_possible_moves;
}
