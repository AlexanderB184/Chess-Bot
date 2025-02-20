cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)

project(ABBOT VERSION 1.1.4)

set(CMAKE_C_COMPILER gcc)

add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-O3)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(bot src/bot.c src/eval.c src/uci.c src/move_order.c src/search.c src/see.c src/transposition_table.c src/worker.c)

target_link_libraries(bot chess)
target_link_libraries(bot pthread)