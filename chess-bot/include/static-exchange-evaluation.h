#include "bot-types.h"

typedef struct {
  score_centipawn_t value;
  sq0x88_t square;
  uint8_t padding;
} attacker_t;

typedef struct {
  int piece_count;
  attacker_t pieces[16];
} attacker_list_t;

void generate_attackers(const chess_state_t* chess_state,
                        attacker_list_t* attackers, attacker_list_t* defenders,
                        sq0x88_t from, sq0x88_t to);

void update_xray_attackers(const chess_state_t* chess_state,
                           attacker_list_t* attackers,
                           attacker_list_t* defenders, sq0x88_t from,
                           sq0x88_t to);

attacker_t next_attacker(attacker_list_t* attackers);

score_centipawn_t static_exchange_evaluation(const chess_state_t* chess_state, move_t move);