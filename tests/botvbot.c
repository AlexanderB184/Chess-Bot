#include "../chess-gui/include/uci-api.h"

colour_t play_match(bot_interface_t* white_bot, bot_interface_t* black_bot) {
    bot_term_cond_t stop_cond = {.time_limit_ms = 100};
    uci_send_ucinewgame(white_bot);
    uci_send_ucinewgame(black_bot);
    chess_state_t match = {};
    load_start_position(&match);
    bot_interface_t* bot_to_move = white_bot;
    while (!is_gameover(&match)) {
        uci_send_position(bot_to_move, &match);
        uci_send_go(bot_to_move, NULL, 0, 0, NULL, &stop_cond);
        move_t bestmove, ponder;
        uci_read_bestmove(bot_to_move, &match, &bestmove, &ponder);
        make_move(&match, bestmove);
        bot_to_move = (bot_to_move == white_bot) ? black_bot : white_bot;
    }
    return get_winner(&match) ;
    
}

int main(int argc, char const *argv[])
{
    if (argc < 4) {
        printf("usage: %s [bot 1] [bot 2] [nmatches]\n", argv[0]);
        exit(0);
    }
    const char* bot1 = argv[1];
    const char* bot2 = argv[2];
    int nmatches = atoi(argv[3]);
    bot_interface_t bots[2];
    create_bot(&bots[0], bot1);
    create_bot(&bots[1], bot2);
    uci_send_uci(&bots[0]);
    uci_send_uci(&bots[1]);
    int wins[2] = {0, 0};
    int outcome;
    for (int i = 0; i < nmatches / 2; i++) {
        outcome = play_match(&bots[0], &bots[1]);
        if (outcome == WHITE) {
            wins[0]++;
        } else if (outcome == BLACK) {
            wins[1]++;
        }
        outcome = play_match(&bots[1], &bots[0]);
        if (outcome == WHITE) {
            wins[1]++;
        } else if (outcome == BLACK) {
            wins[0]++;
        }
    }
    printf("bot 1 wins: %d, bot 2 wins: %d\n", wins[0], wins[1]);
    return 0;
}
