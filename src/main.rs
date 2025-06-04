mod draw;
mod rules;

use draw::*;
use rules::*;
use raylib::prelude::*;
use raylib::consts::KeyboardKey::*;
use std::collections::HashMap;

const NULL_MOVE: Move = Move {start_x: 99, start_y: 99, end_x: 99, end_y: 99};

fn get_player_move(rl: &mut RaylibHandle, game_position: &mut Position, thread: &RaylibThread, piece_images_map: &HashMap<PieceNames, Texture2D>) -> Move {
    let mut previous_mouse_x = 99;
    let mut previous_mouse_y = 99;
    loop {
        let mouse_x = (rl.get_mouse_x() / 60) as usize;
        let mouse_y = (rl.get_mouse_y() / 60) as usize;

        draw_board(rl, &thread, game_position, &piece_images_map);

        if rl.is_key_down(KEY_Q) {
            return NULL_MOVE;
        }

        if rl.is_mouse_button_pressed(MouseButton::MOUSE_BUTTON_LEFT) {
            if mouse_x > 7 || mouse_y > 7{
                continue;
            }
            if game_position.turn != (game_position.board[mouse_x][7 - mouse_y].piece.colour as i8) {
                continue;
            }
            remove_legal_highlights(game_position);
            highlight_piece_legal_moves(game_position, mouse_x as i32, (7-mouse_y) as i32);
            game_position.dragged_piece = game_position.board[mouse_x][7 - mouse_y].piece.piece_name;
            previous_mouse_x = mouse_x;
            previous_mouse_y = mouse_y;
        }


        if rl.is_mouse_button_released(MouseButton::MOUSE_BUTTON_LEFT) {
            game_position.dragged_piece = PieceNames::Blank;
            if mouse_x > 7 || mouse_y > 7{
                continue;
            }
            if previous_mouse_x > 7 || previous_mouse_y > 7{
                continue;
            }
            if previous_mouse_x == mouse_x && previous_mouse_y == mouse_y {
                continue;
            }
            remove_legal_highlights(game_position);
            if game_position.turn != (game_position.board[previous_mouse_x][7 - previous_mouse_y].piece.colour as i8) {
                continue; // is our turn
            }
            if game_position.turn == (game_position.board[mouse_x][7 - mouse_y].piece.colour as i8) {
                continue; // can not capture own piece
            }

            // is legal
            let m = Move {start_x: previous_mouse_x, start_y: 7 - previous_mouse_y, end_x: mouse_x, end_y: 7 - mouse_y};
            let all_legal_moves = get_all_legal_moves(*game_position);
            if !all_legal_moves.contains(&m) {
                continue;
            }

            return m;
        }
    }
}

fn main() {
    let (mut rl, thread) = raylib::init()
        //.size(960, 720)
        .size(480, 480)
        .title("Chess")
        .build();

    let piece_images_map: HashMap<PieceNames, Texture2D> = initialise_images(&mut rl, &thread);
    let game_board = [[Square {piece: BLANK_PIECE, highlight: Highlights::NORMAL}; 8]; 8];
    let mut game_position = Position { 
        board: game_board, 
        turn: 1, 
        dragged_piece: PieceNames::Blank, 
        previous_move: NULL_MOVE, 
        en_passant: -1,
        can_white_castle_kingside: true, 
        can_white_castle_queenside: true,
        can_black_castle_kingside: true, 
        can_black_castle_queenside: true,
    };
    initialise_board(&mut game_position);

    while !rl.window_should_close() {
        let my_move: Move = get_player_move(&mut rl, &mut game_position, &thread, &piece_images_map); 
        if my_move == NULL_MOVE {
            break;
        }
        move_piece(&mut game_position, my_move);
        game_position.turn *= -1;
        game_position.previous_move = my_move;
        draw_board(&mut rl, &thread, &mut game_position, &piece_images_map);
    }
}
