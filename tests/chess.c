#include "../chess-gui/include/uci-api.h"
#include "../chess-gui/include/gui.h"

#include <errno.h>

int player_move(FILE* player, const chess_state_t* position, move_t* move) {
    char buffer[256];
    size_t size = sizeof(buffer);
    if (fgets(buffer, size, stdin) == NULL) {
        if (feof(stdin)) {
        fprintf(stderr, "reached eof");
        exit(0);
        }
        fprintf(stderr, "failed to read from stdin: %s", strerror(errno));
        exit(1);
    }
    return read_algebraic_notation(buffer, size, position, move);
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        printf("usage: %s [bot 1]\n", argv[0]);
        exit(0);
    }

    const char* name = argv[1];
    bot_interface_t bot;
    chess_state_t position = {};
    bot_term_cond_t stop_cond = {.time_limit_ms = 100};
    load_start_position(&position);
    create_bot(&bot, name);
    uci_send_uci(&bot);
    uci_send_ucinewgame(&bot);

    int turn = 0;
    display_board(&position);
    while (!is_gameover(&position)) {
        if (turn) {
            uci_send_position(&bot, &position);
            uci_send_go(&bot, NULL, 0, 0, NULL, &stop_cond);
            move_t bestmove, ponder;
            uci_read_bestmove(&bot, &position, &bestmove, &ponder);
            make_move(&position, bestmove);
        } else {
            move_t move;
            while (player_move(stdin, &position, &move) < 0) printf("illegal move\n");
            make_move(&position, move);
        }
        turn = !turn;
        display_board(&position);
        
    }
    switch (get_winner(&position)) {
        case WHITE:
            printf("White wins!!!\n"); break;
        case BLACK:
        printf("Black wins!!!\n"); break;
        case EMPTY:
        printf("Draw!!!\n"); break;

    }
    return 0;
}
