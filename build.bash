# Define flags and file lists
CFLAGS="-Wall -Werror -O3 -std=c11 -DNDEBUG -D_POSIX_C_SOURCE=199309L"
CHESS_FILES="attack.c chess_gui.c fen.c move_generation.c move_validation.c move.c perft.c tournament.c"
BOT_DIR="./bots/"
BOT_FILES="iterative_deepening.c quiescence.c static_eval.c transposition_table.c"
MYLIB_DIR="./mylib/"
MYLIB_FILES="Arena.c String.c"

# Prefix bot and mylib files with their respective directories
FULL_BOT_FILES=$(echo $BOT_FILES | sed "s~[^ ]*~$BOT_DIR&~g")
FULL_MYLIB_FILES=$(echo $MYLIB_FILES | sed "s~[^ ]*~$MYLIB_DIR&~g")

# Compile all files
gcc $CFLAGS $CHESS_FILES $FULL_BOT_FILES $FULL_MYLIB_FILES -o test_tournament