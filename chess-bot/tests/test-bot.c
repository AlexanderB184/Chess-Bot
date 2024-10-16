#include "../include/bot.h"
#include <stdio.h>
#include <stdlib.h>

bot_t bot;

int main() {
    int ret;
    bot_init(&bot, NULL);
    char* start_position = "r1bq1rk1/pppp1ppp/1bn4B/4P3/2BP4/5N1P/PP3PP1/RN1Q1RK1 b - - 0 10";
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