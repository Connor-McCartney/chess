use crate::rules::*;
use crate::draw::*;
use rand::Rng;
use std::collections::HashMap;
use std::cmp::{min, max};

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

pub fn get_engine_move(game_position: &mut Position) -> Move {
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
