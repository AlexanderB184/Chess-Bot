#include "../chess-lib/include/chess-lib.h"

// load fen from pgn
// iterate through moves
// for each move
//  for each piece of winning player inc piece-square table counter
//  for each piece of losing player dec piece-square table counter
// divide each square by number of positions to get a +/- win score factor
// convert winscore factor to pawn score
// save in c array literal format
long positions = 0;
long counters[6][64] = {};

sq0x88_t flip_square(sq0x88_t square) {
    int file = sq0x88_to_file07(square);
    int rank = sq0x88_to_rank07(square);
    return rankfile_to_sq0x88(7-rank, file);
}

void inc(piece_t piece, sq0x88_t square, colour_t winner) {
    if (piece & BLACK) {
        square = flip_square(square);
    }

    int piece_off = 0;
    switch (piece & PIECE_MASK) {
        case PAWN:   piece_off = 0; break;
        case KNIGHT: piece_off = 1; break;
        case BISHOP: piece_off = 2; break;
        case ROOK:   piece_off = 3; break;
        case QUEEN:  piece_off = 4; break;
        case KING:   piece_off = 5; break;
        default: return;
    }
    if ((piece & COLOUR_MASK) == winner) {
        counters[piece_off][sq0x88_to_sq8x8(square)]++;
    } else {
        counters[piece_off][sq0x88_to_sq8x8(square)]--;
    }
}

void add_pos(const chess_state_t* chess_state, colour_t winner) {
    positions++;
    for (int i = 0; i < 64; i++) {
        sq0x88_t sq = sq8x8_to_sq0x88(i);
        if (piece(chess_state, sq) != EMPTY) {
            inc(piece(chess_state, sq),sq,winner);
        }
    }

}


void add_game() {
    // load start position from fen
    chess_state_t chess_state = {};
    colour_t winner;
    // iterate for each move
    while(1) {
        add_pos(&chess_state, winner);
        move_t move;
        make_move(&chess_state, move);
    }
}

void add_games() {
    for (;;) {
        add_game();
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 64; j++) {
            (float)counters[i][j] / positions;
        }
    }
}