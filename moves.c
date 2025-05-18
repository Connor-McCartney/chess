#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "pieces.h"
#include "moves.h"

int turn = WHITE;

node_t *create_node(move_t move){
    node_t *new_node = malloc(sizeof(node_t));
    new_node->move = move;
    new_node->next = NULL;
    return new_node;
}

void destroy_list(node_t *head){
    node_t *current = head;
    node_t *next = current;
    while (current->next != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(current);

}

void push_end(node_t *head, move_t move) {
    node_t *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    node_t *new_node = create_node(move);
    current->next = new_node;
}


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


node_t *get_possible_rook_moves(square_t board[8][8], int x, int y) {
    assert(x >= 0);
    assert(x >= 0);
    assert(y <= 7);
    assert(x <= 7);
    assert(board[x][y].piece.piece == ROOK);
    assert(board[x][y].piece.colour != EMPTY);

    int col = board[x][y].piece.colour;
    int i;

    node_t *possible_moves = malloc(sizeof(node_t));
    possible_moves->next = NULL;

    // up
    i = y;
    while (true) {
        i += col;
        if (i > 7 || i < 0) {
            break;
        }
        if (board[x][i].piece.colour != col) {
            move_t move = {x, y, x, i};
            push_end(possible_moves, move);
        }
        if (board[x][i].piece.colour != EMPTY) {
            break;
        } 
    }
    return possible_moves;
}

node_t *get_possible_king_moves(square_t board[8][8], int x, int y) {
    node_t *possible_moves = malloc(sizeof(node_t));
    possible_moves->next = NULL;
    return possible_moves;
}

void highlight_moves(square_t board[8][8], node_t *possible_moves) {
    node_t *current = possible_moves;
    while (current->next != NULL) {
        current = current->next;
        int end_x = current->move.end_x;
        int end_y = current->move.end_y;
        board[end_x][end_y].highlight = LEGAL;
        if (board[end_x][end_y].piece.colour == EMPTY) {
            board[end_x][end_y].piece = dot;
        }
    }
}


void highlight_legal_moves(square_t board[8][8], int x, int y) {
    piece_t piece = board[x][y].piece;

    if (piece.colour != turn) {
        return;
    }

    node_t *possible_moves = NULL;
    switch (piece.piece) {
        case ROOK:
            possible_moves = get_possible_rook_moves(board, x, y);
            break;
        case KING:
            possible_moves = get_possible_king_moves(board, x, y);
            break;
    }

    assert (possible_moves != NULL);
    highlight_moves(board, possible_moves);
    destroy_list(possible_moves);
}
