#include "../include/gui.h"

#include <stdio.h>

int display_board(const chess_state_t* position) {
    fprintf(stdout, "\n   | A | B | C | D | E | F | G | H ");
    for (int r = 7; r >= 0; r --) {
        fprintf(stdout, "\n---+---+---+---+---+---+---+---+---\n %c ", r + '1');
        for (int c = 0; c < 8; c ++) {
            putc('|', stdout);
            piece_t p = position->board[rankfile_to_sq0x88(r, c)];
            switch (p) {
                case EMPTY: putc(' ', stdout); break;
                case WHITE_PAWN:    fprintf(stdout, "♙"); break;
                case WHITE_KNIGHT:  fprintf(stdout, "♘"); break;
                case WHITE_BISHOP:  fprintf(stdout, "♗"); break;
                case WHITE_ROOK:    fprintf(stdout, "♖"); break;
                case WHITE_QUEEN:   fprintf(stdout, "♕"); break;
                case WHITE_KING:    fprintf(stdout, "♔"); break;
                case BLACK_PAWN:    fprintf(stdout, "♟"); break;
                case BLACK_KNIGHT:  fprintf(stdout, "♞"); break;
                case BLACK_BISHOP:  fprintf(stdout, "♝"); break;
                case BLACK_ROOK:    fprintf(stdout, "♜"); break;
                case BLACK_QUEEN:   fprintf(stdout, "♛"); break;
                case BLACK_KING:    fprintf(stdout, "♚"); break;
            }
            putc(' ', stdout);
            putc(' ', stdout);

        }
        if (r != 0) {
        } else {
            fprintf(stdout, "\n");
        }
    }
    fflush(stdout);
    return 0;
}