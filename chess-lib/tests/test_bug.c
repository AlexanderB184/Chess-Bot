#include "../include/chess-lib.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    chess_state_t chess_state = {};
    load_start_position(&chess_state);
    make_move(&chess_state, move(f2, f4, DOUBLE_PAWN_PUSH));
    make_move(&chess_state, move(b8, c6, QUIET_MOVE));
    make_move(&chess_state, move(e1, f2, QUIET_MOVE));
    make_move(&chess_state, move(e7, e5, DOUBLE_PAWN_PUSH));
    make_move(&chess_state, move(f2, f3, QUIET_MOVE));
    make_move(&chess_state, move(f8, c5, QUIET_MOVE));
    make_move(&chess_state, move(d1, e1, QUIET_MOVE));
    move_t moves[256];
    size_t move_count = generate_legal_moves(&chess_state, moves);
    for (int i = 0; i < move_count; i++) {
    printf("from: %d to: %d flags: %d\n", get_from(moves[i]),
            get_to(moves[i]), get_flags(moves[i]));
    }
    

    return 0;
}
