#include "runbot.h"

int main(int argc, const char** argv) {
    if (argc != 3) {
        printf("usage: %s [baseline version] [new version]\n", argv[0]);
        exit(0);
    }
    bot_interface_t bot_1, bot_2;
    spawn_bot(argv[1], &bot_1);
    spawn_bot(argv[2], &bot_2);

    send_command(&bot_1, "uci");
    send_command(&bot_1, "ucinewposition");
    send_command(&bot_1, "isready");

    send_command(&bot_2, "uci");
    send_command(&bot_2, "ucinewposition");
    send_command(&bot_1, "isready");

    
}