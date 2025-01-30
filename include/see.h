#ifndef STATIC_EXCHANGE_EVALUATION_H
#define STATIC_EXCHANGE_EVALUATION_H

#include "bot.h"

typedef struct {
  score_cp_t value;
  sq0x88_t square;
  uint8_t padding;
} attacker_t;

typedef struct {
  int piece_count;
  attacker_t pieces[16];
} attacker_list_t;


score_cp_t static_exchange_evaluation(const chess_state_t* chess_state,
                                             move_t move);

#endif