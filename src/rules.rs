use std::usize;
use crate::draw::*;

pub const NULL_MOVE: Move = Move {
    start_x: 99,
    start_y: 99, 
    end_x: 99, 
    end_y: 99,
    start_piece: BLANK_PIECE,
    end_piece: BLANK_PIECE,
};

pub fn create_move(game_position: &Position, start_x: usize, start_y: usize, end_x: usize, end_y: usize) -> Move {
    return Move {
        start_x,
        start_y,
        end_x,
        end_y,
        start_piece: game_position.board[start_x][start_y].piece,
        end_piece: game_position.board[end_x][end_y].piece,
    };
}

pub fn is_double_pawn_move(m: Move) -> bool {
    if m.start_piece.piece_name == PieceNames::WhitePawn &&
       m.start_y == 1 && m.end_y == 3 {
        return true;
    }
    if m.start_piece.piece_name == PieceNames::BlackPawn &&
       m.start_y == 6 && m.end_y == 4 {
        return true;
    }
    return false;
}

pub fn is_move_en_passant(m: Move) -> bool {
    let start = m.start_piece;
    if start.piece_type != PieceTypes::PAWN {
        return false;
    }
    if m.start_x == m.end_x  {
        return false;
    }
    if m.end_piece != BLANK_PIECE {
        return false;
    }
    if start.colour == Colours::WHITE {
        if m.start_y != 4 || m.end_y != 5 {
            return false;
        }
    }
    if start.colour == Colours::BLACK {
        if m.start_y != 3 || m.end_y != 2 {
            return false;
        }
    }
    return true;
}

fn move_piece(game_position: &mut Position, m: Move) {
    // remove previous highlight
    let l = game_position.move_history.len();
    if l != 0 {
        let prev_move = game_position.move_history[l-1];
        if prev_move != NULL_MOVE {
            game_position.board[prev_move.start_x][prev_move.start_y].highlight = Highlights::NORMAL;
            game_position.board[prev_move.end_x][prev_move.end_y].highlight = Highlights::NORMAL;
        }
    }

    // new previous move highlight
    game_position.board[m.start_x][m.start_y].highlight = Highlights::PREVIOUS;
    game_position.board[m.end_x][m.end_y].highlight = Highlights::PREVIOUS;

    let start = game_position.board[m.start_x][m.start_y].piece;
    let empty = super::draw::Piece { piece_name: PieceNames::Blank, piece_type: PieceTypes::BLANK, colour: Colours::EMPTY}; 

    /////////////////////////////////////////////////////////////////

    let l = game_position.castling_rights_history.len();
    let mut new_castling_rights: Vec<bool>;
    if l == 0 {
        new_castling_rights = vec![true, true, true, true]; // W o-o, B o-o, W o-o-o, B o-o-o
    } else {
        new_castling_rights = game_position.castling_rights_history[l-1].clone(); 
    }


    // if we move a rook we can't castle anymore
    if start.piece_name == PieceNames::WhiteRook {
        if m.start_x == 0 && m.start_y == 0 {
            new_castling_rights[2] = false;
        }
        if m.start_x == 7 && m.start_y == 0 {
            new_castling_rights[0] = false;
        }
    }
    if start.piece_name == PieceNames::BlackRook {
        if m.start_x == 0 && m.start_y == 7 {
            new_castling_rights[3] = false;
        }
        if m.start_x == 7 && m.start_y == 7 {
            new_castling_rights[1] = false;
        }
    }

    // if we move a king we can't castle anymore
    if start.piece_name == PieceNames::WhiteKing {
        new_castling_rights[2] = false;
        new_castling_rights[0] = false;
    }
    if start.piece_name == PieceNames::BlackKing {
        new_castling_rights[3] = false;
        new_castling_rights[1] = false;
    }

    game_position.castling_rights_history.push(new_castling_rights);
    
    // if a pawn moves 2 squares, it has the possibility of being captured en passant
    if is_double_pawn_move(m) {
        game_position.en_passant = m.start_x as i32;
    } else {
        game_position.en_passant = -1;
    }

    /////////////////////////////////////////////////////////////////


    // I'll just auto-queen instead of making an underpromotion popup
    if start.piece_type == PieceTypes::PAWN && start.colour == Colours::WHITE && m.end_y == 7 {
        game_position.board[m.end_x][m.end_y].piece = WHITE_QUEEN_PIECE;
        game_position.board[m.start_x][m.start_y].piece = empty;
        return;
    }
    if start.piece_type == PieceTypes::PAWN && start.colour == Colours::BLACK && m.end_y == 0 {
        game_position.board[m.end_x][m.end_y].piece = BLACK_QUEEN_PIECE;
        game_position.board[m.start_x][m.start_y].piece = empty;
        return;
    }

    // castling
    if start.piece_type == PieceTypes::KING {
        let yy: usize;
        let new_rook: Piece;
        if start.colour == Colours::WHITE {
            yy = 0;
            new_rook = WHITE_ROOK_PIECE;
        } else {
            yy = 7;
            new_rook = BLACK_ROOK_PIECE;
        }

        if m.start_x == 4 && m.start_y == yy && m.end_y == yy {
            if m.end_x == 6 { // kingside
                game_position.board[m.end_x][m.end_y].piece = start ;
                game_position.board[m.start_x][m.start_y].piece = empty ;
                game_position.board[7][m.end_y].piece = empty; // rook disappears
                game_position.board[5][m.end_y].piece = new_rook; // rook moves to other side
                return;
            }
            if m.end_x == 2 { // queenside
                game_position.board[m.end_x][m.end_y].piece = start;
                game_position.board[m.start_x][m.start_y].piece = empty;
                game_position.board[0][m.end_y].piece = empty; // rook disappears
                game_position.board[3][m.end_y].piece = new_rook; // rook moves to other side
                return;
            }
        }
    }

    // regular moves
    game_position.board[m.end_x][m.end_y].piece = start; // piece at end becomes piece at start
    game_position.board[m.start_x][m.start_y].piece = empty; // piece at start becomes empty


    // en passant
    if is_move_en_passant(m) {
        game_position.board[m.end_x][m.start_y].piece = empty; 
    }
}

pub fn play_move(game_position: &mut Position, m: Move) {
    move_piece(game_position, m);
    game_position.move_history.push(m);
    game_position.turn *= -1;
}



fn get_possible_knight_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let col = game_position.board[x as usize][y as usize].piece.colour;
    let mut possible_moves = vec![];
    for i in -2..3_i32 {
        for j in -2..3_i32 {
            if i==0 || j==0 || i32::abs(i)==i32::abs(j) {
                continue;
            }
            let yy = y + i;
            let xx = x + j;
            if xx > 7 || yy > 7 || xx < 0 || yy < 0 {
                continue;
            }
            if game_position.board[xx as usize][yy as usize].piece.colour != col {
                let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                possible_moves.push(m);
            }
            if game_position.board[xx as usize][yy as usize].piece.colour != Colours::EMPTY {
                continue;
            }
        }
    }
    return possible_moves;
}

fn get_possible_rook_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let col = game_position.board[x as usize][y as usize].piece.colour;
    let mut possible_moves = vec![];
    let up_down: Vec<i32> = vec![0, 0, -1, 1];
    let left_right: Vec<i32> = vec![-1, 1, 0, 0];
    for i in 0..4_usize {
        let mut yy = y;
        let mut xx = x;
        loop {
            xx += left_right[i];
            yy += up_down[i];
            if xx > 7 || yy > 7 || xx < 0 || yy < 0 {
                break;
            }
            if game_position.board[xx as usize][yy as usize].piece.colour != col {
                let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                possible_moves.push(m);
            }
            if game_position.board[xx as usize][yy as usize].piece.colour != Colours::EMPTY {
                break;
            }
        }
    }
    return possible_moves;
}


fn get_possible_bishop_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let col = game_position.board[x as usize][y as usize].piece.colour;
    let mut possible_moves = vec![];

    for i in [-1, 1] {
        for j in [-1, 1] {
            let mut yy = y;
            let mut xx = x;
            loop {
                xx += i;
                yy += j;
                if xx > 7 || yy > 7 || xx < 0 || yy < 0 {
                    break;
                }
                if game_position.board[xx as usize][yy as usize].piece.colour != col {
                    let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                    possible_moves.push(m);
                }
                if game_position.board[xx as usize][yy as usize].piece.colour != Colours::EMPTY {
                    break;
                }
            }
        }
    }
    return possible_moves
}

fn get_possible_pawn_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let col = game_position.board[x as usize][y as usize].piece.colour;
    let mut possible_moves = vec![];

    if (col == Colours::WHITE && y == 1) || (col == Colours::BLACK && y == 6) {
        // forward 1 or 2 squares
        let mut yy = y;
        for _ in 0..2 {
            yy += col as i32;
            if game_position.board[x as usize][yy as usize].piece.colour == Colours::EMPTY {
                let m = create_move(game_position, x as usize, y as usize, x as usize, yy as usize);
                possible_moves.push(m);
            } else {
                break;
            }
        }
    } else {
        // forward only 1 square
        let mut yy = y;
        yy += col as i32;
        if yy <= 7 && yy >= 0 {
            if game_position.board[x as usize][yy as usize].piece.colour == Colours::EMPTY {
                let m = create_move(game_position, x as usize, y as usize, x as usize, yy as usize);
                possible_moves.push(m);
            }
        }
    }

    // captures
    for i in [-1, 1] {
        let xx = x + i;
        let yy = y + col as i32;
        if xx <= 7 && yy <= 7 && xx >= 0 && yy >= 0 {
            if game_position.board[xx as usize][yy as usize].piece.colour as i32 == -(col as i32) {
                let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                possible_moves.push(m);
            }
            // en passant
            if xx == game_position.en_passant {
                if (y == 3 && col == Colours::BLACK) || (y == 4 && col == Colours::WHITE) {
                    let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                    possible_moves.push(m);
                }
            }
        }
    }
    return possible_moves;
}


fn get_possible_queen_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let col = game_position.board[x as usize][y as usize].piece.colour;
    let mut possible_moves = vec![];
    for i in [-1, 0, 1] {
        for j in [-1, 0, 1] {
            let mut yy = y;
            let mut xx = x;
            loop {
                xx += i;
                yy += j;
                if xx > 7 || yy > 7 || xx < 0 || yy < 0 {
                    break
                }
                if game_position.board[xx as usize][yy as usize].piece.colour != col {
                    let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                    possible_moves.push(m);
                }
                if game_position.board[xx as usize][yy as usize].piece.colour != Colours::EMPTY {
                    break
                }
            }
        }
    }
    return possible_moves;
}


fn get_possible_king_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let col = game_position.board[x as usize][y as usize].piece.colour;
    let mut possible_moves = vec![];
    for i in [-1, 0, 1] {
        for j in [-1, 0, 1] {
            let yy = y + i;
            let xx = x + j;
            if xx > 7 || yy > 7 || xx < 0 || yy < 0 {
                continue;
            }
            if game_position.board[xx as usize][yy as usize].piece.colour != col {
                let m = create_move(game_position, x as usize, y as usize, xx as usize, yy as usize);
                possible_moves.push(m);
            }
            if game_position.board[xx as usize][yy as usize].piece.colour != Colours::EMPTY {
                continue;
            }
        }
    }
    let l = game_position.castling_rights_history.len();
    let castling_rights: Vec<bool>;
    if l == 0 {
        castling_rights = vec![true, true, true, true]; 
    } else {
        castling_rights = game_position.castling_rights_history[l-1].clone(); 
    }
    let can_white_castle_kingside  = castling_rights[0];
    let can_black_castle_kingside  = castling_rights[1];
    let can_white_castle_queenside = castling_rights[2];
    let can_black_castle_queenside = castling_rights[3];
    // castle kingside
    if col == Colours::WHITE {
        if game_position.board[5][0].piece.colour == Colours::EMPTY &&
                game_position.board[6][0].piece.colour == Colours::EMPTY &&
                can_white_castle_kingside {
            let m = create_move(game_position, x as usize, y as usize, (x+2) as usize, y as usize);
            possible_moves.push(m);
        }
    }
    if col == Colours::BLACK {
        if game_position.board[5][7].piece.colour == Colours::EMPTY &&
                game_position.board[6][7].piece.colour == Colours::EMPTY &&
                can_black_castle_kingside {
            let m = create_move(game_position, x as usize, y as usize, (x+2) as usize, y as usize);
            possible_moves.push(m);
        }
    }
    // castle queenside
    if col == Colours::WHITE {
        if game_position.board[3][0].piece.colour == Colours::EMPTY &&
                game_position.board[2][0].piece.colour == Colours::EMPTY &&
                can_white_castle_queenside {
            let m = create_move(game_position, x as usize, y as usize, (x-2) as usize, y as usize);
            possible_moves.push(m);
        }
    }
    if col == Colours::BLACK {
        if game_position.board[3][7].piece.colour == Colours::EMPTY &&
                game_position.board[2][7].piece.colour == Colours::EMPTY &&
                can_black_castle_queenside {
            let m = create_move(game_position, x as usize, y as usize, (x-2) as usize, y as usize);
            possible_moves.push(m);
        }
    }
    return possible_moves
}

fn get_piece_possible_moves(game_position: &Position, x: i32, y: i32) -> Vec<Move> {
    let possible_moves = match game_position.board[x as usize][y as usize].piece.piece_type {
        PieceTypes::KNIGHT => get_possible_knight_moves(game_position, x, y),
        PieceTypes::ROOK => get_possible_rook_moves(game_position, x, y), 
        PieceTypes::QUEEN => get_possible_queen_moves(game_position, x, y), 
        PieceTypes::KING => get_possible_king_moves(game_position, x, y), 
        PieceTypes::BISHOP => get_possible_bishop_moves(game_position, x, y), 
        PieceTypes::PAWN => get_possible_pawn_moves(game_position, x, y),
        PieceTypes::BLANK => unreachable!()
    };
    return possible_moves;
}

// just used for is_check
fn get_all_possible_moves(game_position: &Position) -> Vec<Move> {
    let mut all_possible_moves = vec![];
    for x in 0..8 {
        for y in 0..8 {
            let piece = game_position.board[x][y].piece;
            if piece.colour as i8 != game_position.turn {
                continue
            }
            for possible_move in get_piece_possible_moves(game_position, x as i32, y as i32) {
                all_possible_moves.push(possible_move)
            }
        }
    }
    return all_possible_moves;
}

fn is_square_check(game_position: &mut Position, x: i32, y: i32) -> bool {
    let possible_moves = get_all_possible_moves(game_position);
    for possible_move in possible_moves {
        if possible_move.end_x == x as usize && possible_move.end_y == y as usize {
            return true;
        }
    }
    return false;
}

fn get_king_position(game_position: &Position) -> Vec<usize> {
    for x in 0..8 {
        for y in 0..8 {
            if game_position.board[x][y].piece.piece_type == PieceTypes::KING &&
                game_position.board[x][y].piece.colour as i8 == -game_position.turn {
                return vec![x, y];
            }
        }
    }
    unreachable!();
}

fn is_check(game_position: &mut Position) -> bool {
    let king_position = get_king_position(game_position);
    return is_square_check(game_position, king_position[0] as i32, king_position[1] as i32)
}

pub fn undo_move(game_position: &mut Position) {
    let l = game_position.move_history.len();
    if l == 0 {
        return;
    }

    game_position.turn *= -1;
    game_position.castling_rights_history.pop();
    let m = game_position.move_history.pop().unwrap();

    // remove current highlight 
    game_position.board[m.start_x][m.start_y].highlight = Highlights::NORMAL;
    game_position.board[m.end_x][m.end_y].highlight = Highlights::NORMAL;

    // normal move
    game_position.board[m.start_x][m.start_y].piece = m.start_piece;
    game_position.board[m.end_x][m.end_y].piece = m.end_piece;

    if m.start_piece.piece_name == PieceNames::WhiteKing && m.start_x == 4 {
        // white kingside castle
        if m.end_x == 6 {
            game_position.board[5][0].piece = BLANK_PIECE;
            game_position.board[7][0].piece = WHITE_ROOK_PIECE;
        }

        // white queenside castle
        if m.end_x == 2 {
            game_position.board[3][0].piece = BLANK_PIECE;
            game_position.board[0][0].piece = WHITE_ROOK_PIECE;
        }
    }

    if m.start_piece.piece_name == PieceNames::BlackKing && m.start_x == 4 {
        // black kingside castle
        if m.end_x == 6 {
            game_position.board[5][7].piece = BLANK_PIECE;
            game_position.board[7][7].piece = BLACK_ROOK_PIECE;
        }

        // black queenside castle
        if m.end_x == 2 {
            game_position.board[3][7].piece = BLANK_PIECE;
            game_position.board[0][7].piece = BLACK_ROOK_PIECE;
        }
    }

    // en passant
    if is_move_en_passant(m) {
        if game_position.turn == -1 {
            game_position.board[m.end_x][m.start_y].piece = BLACK_PAWN_PIECE;
        } else if game_position.turn == 1 {
            game_position.board[m.end_x][m.start_y].piece = WHITE_PAWN_PIECE;
        }

        game_position.en_passant = m.end_x as i32;
        return;
    } else {
        if l >= 2 {
            let prev_move = game_position.move_history[l-2];
            if is_double_pawn_move(prev_move) {
                game_position.en_passant = prev_move.start_x as i32;
            } else {
                game_position.en_passant = -1;
            }
        } else {
            game_position.en_passant = -1;
        }
    }

    // add previous highlight
    if l >= 2 {
        let m2 = game_position.move_history[l-2];
        game_position.board[m2.start_x][m2.start_y].highlight = Highlights::PREVIOUS;
        game_position.board[m2.end_x][m2.end_y].highlight = Highlights::PREVIOUS;
    }
}

fn get_piece_legal_moves(game_position: &mut Position, x: i32, y: i32) -> Vec<Move> {
    let possible_moves = get_piece_possible_moves(game_position, x, y);
    let mut legal_moves = vec![];
    for possible_move in possible_moves {
        play_move(game_position, possible_move);

        // if castling, ensure we don't move through check
        let start = possible_move.start_piece;
        if start.piece_type == PieceTypes::KING {
            let yy: i32;
            if start.colour == Colours::WHITE {
                yy = 0;
            } else {
                yy = 7;
            }
            if possible_move.start_x == 4 && possible_move.start_y as i32 == yy && possible_move.end_y as i32 == y {
                if possible_move.end_x == 6 { // kingside
                    if is_square_check(game_position, 4, yy) || is_square_check(game_position, 5, yy) {
                        undo_move(game_position);
                        continue
                    }
                }
                if possible_move.end_x == 2 { // queenside
                    if is_square_check(game_position, 4, yy) || is_square_check(game_position, 3, yy) {
                        undo_move(game_position);
                        continue;
                    }
                }
            }
        }

        // ensure we don't move into check
        if is_check(game_position) {
            undo_move(game_position);
            continue;
        }

        undo_move(game_position);
        legal_moves.push(possible_move);
    }
    return legal_moves;
}

pub fn get_all_legal_moves(game_position: &mut Position) -> Vec<Move> {
    let mut all_legal_moves = vec![];
    for x in 0..8 {
        for y in 0..8 {
            let piece = game_position.board[x][y].piece;
            if piece.colour as i8 != game_position.turn {
                continue;
            }
            let legal_moves = get_piece_legal_moves(game_position, x as i32, y as i32);
            for legal_move in legal_moves {
                all_legal_moves.push(legal_move);
            }
        }
    }
    return all_legal_moves;
}

pub fn highlight_piece_legal_moves(game_position: &mut Position, x: i32, y: i32) {
    for legal_move in get_piece_legal_moves(game_position, x, y) {
        let end_x = legal_move.end_x;
        let end_y = legal_move.end_y;
        game_position.board[end_x][end_y].highlight = Highlights::LEGAL;
    }
}
