use raylib::prelude::*;
use std::collections::HashMap;
use crate::rules::*;

#[derive(Copy, Clone, Hash, Eq, PartialEq)]
pub enum PieceNames {
    WhiteKing,
    WhiteQueen,
    WhiteBishop,
    WhiteKnight,
    WhiteRook,
    WhitePawn,
    BlackKing,
    BlackQueen,
    BlackBishop,
    BlackKnight,
    BlackRook,
    BlackPawn,
    Blank,
}

#[derive(Copy, Clone, Hash, Eq, PartialEq)]
pub enum PieceTypes {
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PAWN,
    BLANK,
}

#[derive(Copy, Clone, Eq, PartialEq)]
pub enum Highlights {
    NORMAL,
    PREVIOUS,
    LEGAL,
    CHECK,
}

#[derive(Copy, Clone, Eq, PartialEq, Hash)]
pub enum Colours {
    WHITE = 1,
    EMPTY = 0,
    BLACK = -1
}

#[derive(Copy, Clone, Eq, PartialEq, Hash)]
pub struct Piece {
    pub piece_name: PieceNames,
    pub piece_type: PieceTypes,
    pub colour:Colours,
}

#[derive(Copy, Clone, Eq, PartialEq)]
pub struct Square {
    pub piece: Piece,
    pub highlight: Highlights
}

#[derive(Copy, Clone, Eq, PartialEq, Hash)]
pub struct Move {
    pub start_x: usize,
    pub start_y: usize, 
    pub end_x: usize,
    pub end_y: usize, 
    pub start_piece: Piece, 
    pub end_piece: Piece
}

#[derive(Clone, Eq, PartialEq)]
pub struct Position {
    pub board:  [[Square; 8]; 8],
    pub turn: i8,
    pub dragged_piece: PieceNames,
    pub en_passant: i32,
    pub castling_rights_history: Vec<Vec<bool>>, // W o-o, B o-o, W o-o-o, B o-o-o
    pub move_history: Vec<Move>, // for undo_move
}


pub const WHITE_KING_PIECE: Piece = Piece { piece_name: PieceNames::WhiteKing, piece_type: PieceTypes::KING, colour: Colours::WHITE };
pub const WHITE_QUEEN_PIECE: Piece = Piece { piece_name: PieceNames::WhiteQueen, piece_type: PieceTypes::QUEEN, colour: Colours::WHITE };
pub const WHITE_ROOK_PIECE: Piece = Piece { piece_name: PieceNames::WhiteRook, piece_type: PieceTypes::ROOK, colour: Colours::WHITE };
pub const WHITE_KNIGHT_PIECE: Piece = Piece { piece_name: PieceNames::WhiteKnight, piece_type: PieceTypes::KNIGHT, colour: Colours::WHITE };
pub const WHITE_BISHOP_PIECE: Piece = Piece { piece_name: PieceNames::WhiteBishop, piece_type: PieceTypes::BISHOP, colour: Colours::WHITE };
pub const WHITE_PAWN_PIECE: Piece = Piece { piece_name: PieceNames::WhitePawn, piece_type: PieceTypes::PAWN, colour: Colours::WHITE };

pub const BLACK_KING_PIECE: Piece = Piece { piece_name: PieceNames::BlackKing, piece_type: PieceTypes::KING, colour: Colours::BLACK};
pub const BLACK_QUEEN_PIECE: Piece = Piece { piece_name: PieceNames::BlackQueen, piece_type: PieceTypes::QUEEN, colour: Colours::BLACK};
pub const BLACK_ROOK_PIECE: Piece = Piece { piece_name: PieceNames::BlackRook, piece_type: PieceTypes::ROOK, colour: Colours::BLACK};
pub const BLACK_KNIGHT_PIECE: Piece = Piece { piece_name: PieceNames::BlackKnight, piece_type: PieceTypes::KNIGHT, colour: Colours::BLACK};
pub const BLACK_BISHOP_PIECE: Piece = Piece { piece_name: PieceNames::BlackBishop, piece_type: PieceTypes::BISHOP, colour: Colours::BLACK};
pub const BLACK_PAWN_PIECE: Piece = Piece { piece_name: PieceNames::BlackPawn, piece_type: PieceTypes::PAWN, colour: Colours::BLACK};

pub const BLANK_PIECE: Piece = Piece { piece_name: PieceNames::Blank, piece_type: PieceTypes::BLANK, colour: Colours::EMPTY };

pub fn initialise_images(rl: &mut RaylibHandle, thread: &RaylibThread) -> HashMap<PieceNames, Texture2D> {
    let piece_images_map: HashMap<PieceNames, Texture2D> = HashMap::from([
        (PieceNames::WhiteKing, rl.load_texture(&thread, "images/white_king.png").unwrap()),
        (PieceNames::WhiteQueen, rl.load_texture(&thread, "images/white_queen.png").unwrap()),
        (PieceNames::WhiteRook, rl.load_texture(&thread, "images/white_rook.png").unwrap()),
        (PieceNames::WhiteKnight, rl.load_texture(&thread, "images/white_knight.png").unwrap()),
        (PieceNames::WhiteBishop, rl.load_texture(&thread, "images/white_bishop.png").unwrap()),
        (PieceNames::WhitePawn, rl.load_texture(&thread, "images/white_pawn.png").unwrap()),
        (PieceNames::BlackKing, rl.load_texture(&thread, "images/black_king.png").unwrap()),
        (PieceNames::BlackQueen, rl.load_texture(&thread, "images/black_queen.png").unwrap()),
        (PieceNames::BlackRook, rl.load_texture(&thread, "images/black_rook.png").unwrap()),
        (PieceNames::BlackKnight, rl.load_texture(&thread, "images/black_knight.png").unwrap()),
        (PieceNames::BlackBishop, rl.load_texture(&thread, "images/black_bishop.png").unwrap()),
        (PieceNames::BlackPawn, rl.load_texture(&thread, "images/black_pawn.png").unwrap()),
    ]);
    return piece_images_map;
}

pub fn initialise_board(game_position: &mut Position)  {
    for x in 0..8 {
        game_position.board[x][1].piece = WHITE_PAWN_PIECE;
        game_position.board[x][6].piece = BLACK_PAWN_PIECE;
    }
    game_position.board[0][0].piece = WHITE_ROOK_PIECE;
    game_position.board[1][0].piece = WHITE_KNIGHT_PIECE;
    game_position.board[2][0].piece = WHITE_BISHOP_PIECE;
    game_position.board[3][0].piece = WHITE_QUEEN_PIECE;
    game_position.board[4][0].piece = WHITE_KING_PIECE;
    game_position.board[5][0].piece = WHITE_BISHOP_PIECE;
    game_position.board[6][0].piece = WHITE_KNIGHT_PIECE;
    game_position.board[7][0].piece = WHITE_ROOK_PIECE;

    game_position.board[0][7].piece = BLACK_ROOK_PIECE;
    game_position.board[1][7].piece = BLACK_KNIGHT_PIECE;
    game_position.board[2][7].piece = BLACK_BISHOP_PIECE;
    game_position.board[3][7].piece = BLACK_QUEEN_PIECE;
    game_position.board[4][7].piece = BLACK_KING_PIECE;
    game_position.board[5][7].piece = BLACK_BISHOP_PIECE;
    game_position.board[6][7].piece = BLACK_KNIGHT_PIECE;
    game_position.board[7][7].piece = BLACK_ROOK_PIECE;
}

pub fn draw_board(rl: &mut RaylibHandle, thread: &RaylibThread, game_position: &mut Position, piece_images_map: &HashMap<PieceNames, Texture2D>, game_over: bool)  {
    let mouse_x = rl.get_mouse_x();
    let mouse_y = rl.get_mouse_y();
    let mut d = rl.begin_drawing(&thread);
    d.clear_background(Color::from_hex("161512").unwrap());
    let width = 60;
    let mut square_colour: ffi::Color;

    let check: bool;
    game_position.turn *= -1;
    if is_check(game_position) {
        check = true;
        let king_position = get_king_position(game_position);
        game_position.board[king_position[0]][king_position[1]].highlight = Highlights::CHECK;
    } else {
        check = false;
    }
    game_position.turn *= -1;

    for x in 0..8 {
        for y in 0..8 {
            // squares
            if (x+y) % 2 == 0 {
                square_colour = Color::from_hex("b48963").unwrap().into();
            } else {
                square_colour = Color::WHITE.into();
            }
            d.draw_rectangle(x*width, (7-y)*width, width, width, square_colour);

            // highlights 
            match game_position.board[x as usize][y as usize].highlight {
                Highlights::PREVIOUS => {
                    d.draw_rectangle(x*width, (7-y)*width, width, width, Color::YELLOW.alpha(0.5));

                }
                Highlights::LEGAL => {
                    d.draw_rectangle(x*width, (7-y)*width, width, width, Color::GREEN.alpha(0.5));
                }
                Highlights::CHECK=> {
                    d.draw_rectangle(x*width, (7-y)*width, width, width, Color::RED.alpha(0.5));
                }
                Highlights::NORMAL => {
                    // pass
                }
            };

            // pieces
            let piece: PieceNames = game_position.board[x as usize][y as usize].piece.piece_name;
            if piece == PieceNames::Blank {
                continue;
            }
            let piece_image: &Texture2D = piece_images_map.get(&piece).unwrap();
            d.draw_texture(&piece_image, x*60, (7-y)*60, Color::WHITE);

            // mouse dragged piece
            if game_position.dragged_piece != PieceNames::Blank {
                let dragged_piece_image: &Texture2D = piece_images_map.get(&game_position.dragged_piece).unwrap();
                d.draw_texture(&dragged_piece_image, mouse_x-30, mouse_y-30, Color::WHITE);
            }
        }
    }

    if game_over {
        if check {
            d.draw_text("checkmate", 240-130, 240-25, 50, Color::RED);
        } else {
            d.draw_text("stalemate", 240-120, 240-25, 50, Color::RED);
        }
    }
}

pub fn remove_legal_highlights(game_position: &mut Position) {
    for x in 0..8 {
        for y in 0..8 {
            if game_position.board[x][y].highlight == Highlights::LEGAL {
                game_position.board[x][y].highlight = Highlights::NORMAL;
            }
        }
    }
}

