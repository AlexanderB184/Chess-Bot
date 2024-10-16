#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int in_fd[2];
    int out_fd[2];
} bot_pipes_t;

typedef struct {
    pid_t pid;
    FILE* to_bot;
    FILE* from_bot;
    const char* name;
} bot_interface_t;

int spawn_bot(const char * bot_path, bot_interface_t *bot_iface) {
    int in_fd[2];
    int out_fd[2];
    if (pipe(in_fd) == -1 || pipe(out_fd) == -1) {
        perror("pipe failed");
        exit(-1);
    }

    int fork_ret = fork();
    if (fork_ret == -1) {
        perror("fork failed");
        exit(-1);
    }

    if (fork_ret == 0) {
        close(in_fd[1]);
        dup2(in_fd[0], STDIN_FILENO);
        close(in_fd[0]);

        close(out_fd[0]);
        dup2(out_fd[1], STDOUT_FILENO);
        dup2(out_fd[1], STDERR_FILENO);
        close(out_fd[1]);


        execlp(bot_path, bot_path, NULL);
        perror("execvp failed");
        exit(-1);
    }

    close(in_fd[0]);
    close(out_fd[1]);

    bot_iface->pid = fork_ret;

    bot_iface->to_bot = fdopen(in_fd[1], "w");
    bot_iface->from_bot = fdopen(out_fd[0], "r");

    if (!bot_iface->to_bot || !bot_iface->from_bot) {
        perror("fdopen failed");
        exit(-1);
    }

    bot_iface->name = bot_path;

    return fork_ret;
}

int send_command(bot_interface_t* bot_iface, const char * cmd) {
    return fprintf(bot_iface->to_bot, "%s\n", cmd);
}

char * read_response(bot_interface_t* bot_iface, char * buffer, size_t size) {
    return fgets(buffer, size, bot_iface->from_bot);
}


int main(int argc, const char** argv) {
    if (argc != 3) {
        printf("usage: %s [baseline version] [new version]\n", argv[0]);
        exit(0);
    }
    
}