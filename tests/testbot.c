#include "../chess-gui/include/uci-api.h"

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
    create_bot(&bot, name);
    bot.quiet_mode = 0;
    uci_send_uci(&bot);
    do {
        load_start_position(&position);
        uci_send_ucinewgame(&bot);
        while (!is_gameover(&position)) {
            uci_send_position(&bot, &position);
            uci_send_go(&bot, NULL, 0, 0, NULL, &stop_cond);
            move_t bestmove, ponder;
            uci_read_bestmove(&bot, &position, &bestmove, &ponder);
            make_move(&position, bestmove);   
        }
    } while (1);
    return 0;
}
