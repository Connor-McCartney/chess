mod draw;
mod rules;


use rand::Rng;
use draw::*;
use rules::*;
use raylib::prelude::*;
use raylib::consts::KeyboardKey::*;
use std::collections::HashMap;
use std::cmp::{min, max};

fn get_player_move(rl: &mut RaylibHandle, game_position: &mut Position, thread: &RaylibThread, piece_images_map: &HashMap<PieceNames, Texture2D>) -> Move {
    let mut previous_mouse_x = 99;
    let mut previous_mouse_y = 99;
    loop {
        let mouse_x = (rl.get_mouse_x() / 60) as usize;
        let mouse_y = (rl.get_mouse_y() / 60) as usize;

        draw_board(rl, &thread, game_position, &piece_images_map, false);

        if rl.is_key_down(KEY_Q) {
            return NULL_MOVE;
        }
        if rl.is_key_pressed(KEY_U) {
            undo_move(game_position);
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
            let m = create_move(game_position, previous_mouse_x, 7-previous_mouse_y, mouse_x, 7-mouse_y);
            let all_legal_moves = get_all_legal_moves(game_position);
            if !all_legal_moves.contains(&m) {
                continue;
            }

            return m;
        }
    }
}

fn check_game_over(rl: &mut RaylibHandle, thread: &RaylibThread, game_position: &mut Position, piece_images_map: &HashMap<PieceNames, Texture2D>)  -> bool {
    let game_over = get_all_legal_moves(game_position).len() == 0;
    if game_over {
        loop {
            draw_board(rl, &thread, game_position, &piece_images_map, true);
            if rl.is_key_down(KEY_Q) {
                break;
            }
        }
        return true;
    } 
    draw_board(rl, &thread, game_position, &piece_images_map, game_over);
    return false;
}

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(480, 480)
        .title("Chess")
        .build();

    let piece_images_map: HashMap<PieceNames, Texture2D> = initialise_images(&mut rl, &thread);
    let game_board = [[Square {piece: BLANK_PIECE, highlight: Highlights::NORMAL}; 8]; 8];
    let mut game_position = Position { 
        board: game_board, 
        turn: 1, 
        dragged_piece: PieceNames::Blank, 
        en_passant: -1,
        castling_rights_history: vec![], // W o-o, B o-o, W o-o-o, B o-o-o
        move_history: vec![],
    };
    initialise_board(&mut game_position);

    while !rl.window_should_close() {
        // white's turn
        let white_move: Move = get_player_move(&mut rl, &mut game_position, &thread, &piece_images_map); 
        if white_move == NULL_MOVE {
            break;
        }
        /*
        let white_move = get_engine_move(&mut game_position);
        */
        play_move(&mut game_position, white_move);
        if check_game_over(&mut rl, &thread, &mut game_position, &piece_images_map) {
            break;
        }

        // black's turn
        let black_move: Move = get_player_move(&mut rl, &mut game_position, &thread, &piece_images_map); 
        if black_move == NULL_MOVE {
            break;
        }
        /*
        let black_move = get_engine_move(&mut game_position);
        */
        play_move(&mut game_position, black_move);
        if check_game_over(&mut rl, &thread, &mut game_position, &piece_images_map) {
            break;
        }
    }
}



fn evaluate_position(game_position: &mut Position) -> i64{
    let mut score: i64 = 0;
    for x in 0..8 {
        for y in 0..8 {
            match game_position.board[x][y].piece.piece_type {
                PieceTypes::PAWN => {
                    score += game_position.board[x][y].piece.colour as i64 * 1
                }
                PieceTypes::ROOK => {
                    score += game_position.board[x][y].piece.colour as i64 * 5
                }
                PieceTypes::KNIGHT => {
                    score += game_position.board[x][y].piece.colour as i64 * 3
                }
                PieceTypes::BISHOP => {
                    score += game_position.board[x][y].piece.colour as i64 * 3
                }
                PieceTypes::QUEEN=> {
                    score += game_position.board[x][y].piece.colour as i64 * 9
                }
                PieceTypes::KING => {
                    //pass
                }
                PieceTypes::BLANK=> {
                    //pass
                }
            };
        }
    }
    return score
}


fn minimax(position: &mut Position, depth: usize, mut alpha: i64, mut beta: i64, is_white: bool) -> i64 {
    if depth == 0 {
        return evaluate_position(position);
    }
    if is_white {
        let mut max_eval: i64 = -99999;
        for m in get_all_legal_moves(position) {
            play_move(position, m);
            let eval = minimax(position, depth-1, alpha, beta, false);
            undo_move(position);
            max_eval = max(max_eval, eval);
            alpha = max(alpha, eval);
            if beta <= alpha {
                break;
            }
        }
        return max_eval
    } else {
        let mut min_eval = 99999;
        for m in get_all_legal_moves(position) {
            play_move(position, m);
            let eval = minimax(position, depth-1, alpha, beta, true);
            undo_move(position);
            min_eval = min(min_eval, eval);
            beta = min(beta, eval);
            if beta <= alpha {
                break;
            }
        }
        return min_eval;
    }
}

fn get_engine_move(game_position: &mut Position) -> Move {
    let mut rng = rand::rng();
    let legal_moves = get_all_legal_moves(game_position);
    let mut evals: HashMap<Move, i64> = Default::default();
    let mut best_moves: Vec<Move> = vec![];

    for m in legal_moves {
        play_move(game_position, m);
        let eval = minimax(game_position, 3, -99999, 99999, game_position.turn==1);
        undo_move(game_position);
        evals.insert(m, eval);
    }

    if game_position.turn == 1 {
        let max_eval = *evals.values().max().unwrap();
        for (k, v) in evals.iter() {
            if *v == max_eval {
                best_moves.push(*k);
            }
        }
    } else {
        let min_eval = *evals.values().min().unwrap();
        for (k, v) in evals.iter() {
            if *v == min_eval {
                best_moves.push(*k);
            }
        }
    }

    let r = rng.random_range(0..best_moves.len());
    return best_moves[r];
}
