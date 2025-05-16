#include <ncurses.h>
#include <locale.h> 
#include <stdio.h>
#include "pieces.c"

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
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            switch (board[i][j].highlight) {
                case NORMAL:
                    attrset(A_NORMAL);
                    break;
                case CLICKED:
                    attrset(COLOR_PAIR(1));
                    break;
                case VALID_MOVE_DESTINATION:
                    attrset(COLOR_PAIR(2));
                    break;
            }
            piece_t piece = board[i][j].piece;
            mvwaddstr(stdscr, 15 - 2*j, 6 + 4*i, piece.unicode_char);
            refresh();
        }
    }
    attrset(A_NORMAL);
    refresh();
}

void highlight_clicked_piece(square_t board[8][8], MEVENT event) {
    int x, y;
    x = (event.x-4)/4;
    y = 7 - (event.y/2);

    if (event.x < 5 || event.x > 35) {
        // outside end border, invalid
        x = -1; 
    }

    if (event.x % 4 == 0) {
        // touching a border
        x = -1; 
    }

    if (event.y > 15) {
        // outside end border, invalid
        y = -1;
    }

    if (event.y % 2 == 0) {
        // touching a border
        y = -1;
    }

    // remove all previous highlighty
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            board[i][j].highlight = NORMAL;
        }
    }

    // highlight clicked piece
    if (x != -1 && y != -1) {
        board[x][y].highlight = CLICKED;
    }
}

void run(void) {
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    curs_set(0); // hide cursor
    draw_border();
    square_t board[8][8];
    initalise_board(board);
    draw_board(board);

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);

    MEVENT event;
    while (1) {
        int c = getch();

        if (c == 'q') {
            return;
        }

        if (c == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_PRESSED) {
                    // left click
                    highlight_clicked_piece(board, event);

                } 
            }
        }
        draw_board(board);
        refresh();
    }
}


int main(void) {
    setlocale(LC_ALL, ""); // for special chars to work https://stackoverflow.com/questions/34538814/print-unicode-characters-in-c-using-ncurses
    initscr(); // ncurses built-in setup
    run();
    endwin(); // ncurses built-in cleanup
    return 0;
}
