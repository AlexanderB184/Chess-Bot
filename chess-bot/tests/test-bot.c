#include "../include/bot.h"
#include <stdio.h>
#include <stdlib.h>

bot_t bot;

int main() {
    bot_init(&bot, NULL);
    char* movetext[] = {NULL}; // no moves
    char* start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    bot_term_cond_t stop_cond = {.depth_limit_ply = 0, .node_limit_nds = 70000, .time_limit_ms = 100};
    bot_load_position(&bot, start_position, movetext);
    for (int i = 0; i < 10; i++) {
        bot_start(&bot, &stop_cond);
        bot_wait(&bot);
        char buffer[16];
        bot_move(&bot, buffer, sizeof(buffer));
        bot_update_position(&bot, buffer);

        bot_save_stats(&bot, stdout);
    }
    char pgn_buffer[2048];
    write_pgn(pgn_buffer, sizeof(pgn_buffer), &bot.threads[0]->position, "bot v0", "bot v0", start_position);
    printf("%s\n", pgn_buffer);
}