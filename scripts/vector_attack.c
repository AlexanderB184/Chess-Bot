#include <stdio.h>
#include <stdlib.h>

#include "chess-lib/include/chess-lib.h"

enum {
  increment_vector_size = 16 * 15,
  king_increments_count = 8,
  queen_increments_count = 8,
  knight_increments_count = 8,
  bishop_increments_count = 4,
  rook_increments_count = 4,
};

static const sq0x88_t king_increments_list[king_increments_count] = {
    1, -1, -16, 16, -15, -17, 15, 17};
static const sq0x88_t knight_increments_list[knight_increments_count] = {
    18, 14, 33, 31, -14, -18, -31, -33};
static const sq0x88_t queen_increments_list[queen_increments_count] = {
    1, -1, -16, 16, -15, -17, 15, 17};
static const sq0x88_t bishop_increments_list[bishop_increments_count] = {
    -15, -17, 15, 17};
static const sq0x88_t rook_increments_list[rook_increments_count] = {1, -1, -16,
                                                                     16};

sq0x88_t queen_increment_vector[increment_vector_size];
sq0x88_t rook_increment_vector[increment_vector_size];
sq0x88_t bishop_increment_vector[increment_vector_size];
sq0x88_t knight_increment_vector[increment_vector_size];
sq0x88_t king_increment_vector[increment_vector_size];
sq0x88_t distance_vector[increment_vector_size];


int d_file(sq0x88_t sq1, sq0x88_t sq2) {
  return abs(sq0x88_to_file07(sq1) - sq0x88_to_file07(sq2));
}

int d_rank(sq0x88_t sq1, sq0x88_t sq2) {
  return abs(sq0x88_to_rank07(sq1) - sq0x88_to_rank07(sq2));
}

int d_file_rank(sq0x88_t sq1, sq0x88_t sq2) {
  return d_file(sq1, sq2) + d_rank(sq1, sq2);
}

int d_square(sq0x88_t sq1, sq0x88_t sq2) {
  int df = d_file(sq1, sq2);
  int dr = d_rank(sq1, sq2);
  return df > dr ? df : dr;
}

int main(int argc, char const *argv[]) {
  const sq0x88_t const_offset = 119;
  for (sq8x8_t i = 0; i <= 64; i++) {
    for (sq8x8_t j = 0; j <= 64; j++) {
      sq0x88_t sq1 = sq8x8_to_sq0x88(i);
      sq0x88_t sq2 = sq8x8_to_sq0x88(j);
      int d = d_square(sq1, sq2);
      int diff = sq1 - sq2;
      distance_vector[diff + const_offset] = d;
      if (d== 0) continue;
      if ((sq0x88_to_file07(sq1) == sq0x88_to_file07(sq2) ||
          sq0x88_to_rank07(sq1) == sq0x88_to_rank07(sq2)) && sq1 != sq2) {
        rook_increment_vector[diff + const_offset] = diff / d;
        queen_increment_vector[diff + const_offset] = diff / d;
      }
      if (abs(sq0x88_to_file07(sq1) - sq0x88_to_file07(sq2)) ==
              abs(sq0x88_to_rank07(sq1) - sq0x88_to_rank07(sq2)) &&
          sq1 != sq2) {
        bishop_increment_vector[diff + const_offset] = diff / d;
        queen_increment_vector[diff + const_offset] = diff/ d;
      }
      if (d == 1) {
        king_increment_vector[diff + const_offset] = diff;
      }
    }
  }
  for (int i = 0; i < knight_increments_count; i++) {
    printf("%d\n", (sq0x88_t)(const_offset + knight_increments_list[i]));
    knight_increment_vector[(sq0x88_t)(const_offset + knight_increments_list[i])] =
        knight_increments_list[i];
  }
  printf("queen\n");
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%d,", queen_increment_vector[i * 16 + j]);
    }
    printf("\n");
  }
  printf("rook\n");
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%d,", rook_increment_vector[i * 16 + j]);
    }
    printf("\n");
  }
  printf("bishop\n");
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%d,", bishop_increment_vector[i * 16 + j]);
    }
    printf("\n");
  }
  printf("knight\n");
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%d,", knight_increment_vector[i * 16 + j]);
    }
    printf("\n");
  }
  printf("king\n");
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%d,", king_increment_vector[i * 16 + j]);
    }
    printf("\n");
  }
  printf("distance\n");
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 16; j++) {
      printf("%d,", distance_vector[i * 16 + j]);
    }
    printf("\n");
  }
  return 0;
}
