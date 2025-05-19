#include <ncurses.h>
#include <locale.h> 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "pieces.h"
#include "moves.h"

void draw_border(void) {
    static char* border = "\
    \r    ┌───┬───┬───┬───┬───┬───┬───┬───┐\n\
    \r  8 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  7 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  6 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  5 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  4 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  3 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  2 │   │   │   │   │   │   │   │   │\n\
    \r    ├───┼───┼───┼───┼───┼───┼───┼───┤\n\
    \r  1 │   │   │   │   │   │   │   │   │\n\
    \r    └───┴───┴───┴───┴───┴───┴───┴───┘\n\
    \r      a   b   c   d   e   f   g   h  \n\
    \r                                     \n";
    mvwaddstr(stdscr, 0, 0, border);
}

void draw_board(square_t board[8][8]) {
    if (is_check(board) == true) {
        mvwaddstr(stdscr, 20, 3, "check");
        for (int i=0; i<8; i++) {
            for (int j=0; j<8; j++) {
                if (board[i][j].piece.piece == KING && board[i][j].piece.colour == turn) {
                    board[i][j].highlight = CHECK;
                }
            }
        }
    } else {
        mvwaddstr(stdscr, 20, 3, "     ");
    }


    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            switch (board[i][j].highlight) {
                case NORMAL:
                    attrset(A_NORMAL);
                    break;
                case CLICKED:
                    attrset(COLOR_PAIR(1));
                    break;
                case LEGAL:
                    attrset(COLOR_PAIR(2));
                    break;
                case CHECK:
                    attrset(COLOR_PAIR(3));
                    break;
            }
            piece_t piece = board[i][j].piece;
            mvwaddstr(stdscr, 15 - 2*j, 6 + 4*i, piece.unicode_char);
        }
    }
    attrset(A_NORMAL);


    if (turn == WHITE) {
        mvwaddstr(stdscr, 23, 3, "white's turn");
    } else if (turn == BLACK) {
        mvwaddstr(stdscr, 23, 3, "black's turn");
    }

    refresh();
}

void get_mouse_xy(MEVENT event, int mouse_xy[2]) {
    int x = (event.x-4)/4;
    int y = 7 - (event.y/2);

    if (event.x < 5 || event.x > 35 || event.y > 15) {
        // outside end border, invalid
        x = -1; 
        y = -1;
    }

    if (event.x % 4 == 0 || event.y % 2 == 0) {
        // touching a border
        x = -1; 
        y = -1;
    }

    mouse_xy[0] = x;
    mouse_xy[1] = y;
}

void remove_highlights(square_t board[8][8]) {
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            board[i][j].highlight = NORMAL;
            if (board[i][j].piece.piece == DOT) {
                board[i][j].piece = empty;
            }
        }
    }
}


void game_over(square_t board[8][8]) {
    int c;
    if (is_check(board)) {
        mvwaddstr(stdscr, 20, 3, "checkmate");
    } else {
        mvwaddstr(stdscr, 20, 3, "stalemate");
    }
    refresh();
    while (true) {
        c = getch();
        if (c == 'q') {
            return;
        }
    }
}


void human_vs_human(void) {
    square_t board[8][8];
    int previous_x = -1;
    int previous_y = -1;

    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    curs_set(0); // hide cursor
    draw_border();
    initalise_board(board);
    draw_board(board);

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);

    MEVENT event;
    while (true) {
        node_t *legal_moves = get_all_legal_moves(board); 
        int num_possible_moves = list_length(legal_moves);
        if (num_possible_moves == 0) {
            game_over(board);
            return;
        }

        int c = getch();
        if (c == 'q') {
            return;
        }

        if (c == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_PRESSED) {
                    // left click
                    remove_highlights(board);

                    int mouse_xy[2];
                    get_mouse_xy(event, mouse_xy);
                    int x = mouse_xy[0];
                    int y = mouse_xy[1];

                    if (x == -1 || y == -1 ) { // an actual square has been clicked. not the border
                        continue;
                    }

                    piece_t current_piece = board[x][y].piece;
                    piece_t previous_piece = board[previous_x][previous_y].piece;

                    if (board[x][y].piece.colour == turn) {
                        board[x][y].highlight = CLICKED;
                    }

                    highlight_legal_moves(board, x, y);

                    if (previous_piece.colour == turn) { // is our turn
                        if (current_piece.colour != turn) { // can not capture own piece
                            move_t move = {previous_x, previous_y, x, y};
                            if (list_contains(legal_moves, move)) {
                                move_piece(board, move);
                                turn *= -1;
                            }
                        }
                    }
                    previous_x = x;
                    previous_y = y;
                } 
            }
        }

        draw_board(board);
    }
}


void random_vs_random(void) {
    square_t board[8][8];

    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    curs_set(0); // hide cursor
    draw_border();
    initalise_board(board);
    draw_board(board);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);



    while (true) {
        int c = getch();

        if (c == 'q') {
            return;
        }
        remove_highlights(board);


        node_t *legal_moves = get_all_legal_moves(board); 
        int num_possible_moves = list_length(legal_moves);
        if (num_possible_moves == 0) {
            game_over(board);
            return;
        }

        int r = 1 + rand() % num_possible_moves; 
        node_t *current = legal_moves;
        for (int t=0; t<r; t++) {
            current = current->next;
        }
        move_piece(board, current->move);
        turn *= -1;
        draw_board(board);
    }
}


int main(void) {
    srand(1234);
    setlocale(LC_ALL, ""); // for special chars to work https://stackoverflow.com/questions/34538814/print-unicode-characters-in-c-using-ncurses
    initscr(); // ncurses built-in setup

    human_vs_human();
    //random_vs_random();

    endwin(); // ncurses built-in cleanup
    return 0;
}
