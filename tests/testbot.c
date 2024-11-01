#include "chess-gui/include/uci-api.h"

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        printf("usage: %s [bot 1]\n", argv[0]);
        exit(0);
    }
    char* name = argv[1];
    int nmatches = atoi(argv[3]);
    bot_interface_t bot;
    chess_state_t position;
    bot_term_cond_t stop_cond = {.time_limit_ms = 100};
    load_start_position(&position);
    create_bot(&bot, name);
    uci_send_uci(&bot);
    uci_send_position(&bot, &position);
    uci_send_go(&bot, NULL, 0, 0, NULL, &stop_cond);
    return 0;
}
