#include "../include/bot.h"
#include <stdio.h>
#include <stdlib.h>

bot_t bot;

int main() {
    int ret;
    bot_init(&bot, NULL);
    char* start_position = "3Rb3/1p2P1k1/p4np1/1n6/8/8/5PPP/6K1 b - - 3 35";
    bot_term_cond_t stop_cond = {.depth_limit_ply = 0, .node_limit_nds = 0, .time_limit_ms = 100};
    bot_load_position(&bot, start_position, "");
    for (int i = 0; i < 100; i++) {
        bot_start(&bot, &stop_cond);
        bot_wait(&bot);
        char buffer[16];
        ret = bot_move(&bot, buffer, sizeof(buffer));
        if (ret == -1) break;
        printf("move: %s.\n", buffer);
        bot_save_stats(&bot, stdout);
        ret = bot_update_position(&bot, buffer);
        if (ret < 0) break;
    }
}