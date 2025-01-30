cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)

project(ABBOT VERSION 1.1.4)

set(CMAKE_C_COMPILER gcc)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-O3)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(bot chess-bot/src/bot.c chess-bot/src/eval.c chess-bot/src/interface.c chess-bot/src/move_order.c chess-bot/src/search.c chess-bot/src/see.c chess-bot/src/transposition_table.c chess-bot/src/worker.c)

target_link_libraries(bot chess)
target_link_libraries(bot pthread)