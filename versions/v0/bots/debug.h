#ifndef DEBUG_H
#define DEBUG_H

#include <stddef.h>

struct debug_info {
  size_t moves_made;
  size_t transposition_tree_hits, transposition_tree_misses;
  size_t beta_fail_highs;
  size_t total_depth_scanned;
  size_t scanned_boards;
};

#define ENABLE_DEBUG_INFO

#endif

// remove this line to disable debug info

#ifdef ENABLE_DEBUG_INFO

extern struct debug_info* dbg;

#define dbg_header() struct debug_info* dbg;

#define dbg_clr() memset(dbg, 0, sizoef(dbg));

#define dbg_mm() dbg->moves_made++;

#define dbg_tth() dbg->transposition_tree_hits++;

#define dbg_ttm() dbg->transposition_tree_misses++;

#define dbg_bfh() dbg->beta_fail_highs++;

#define dbg_tds() dbg->total_depth_scanned++;

#define dbg_sb() dbg->scanned_boards++;

#define dbg_display(BOT, DBG)                                                  \
  printf(                                                                      \
      "bot: %d\nmoves made: %zu\ntransposition tree hits: %zu\ntransposition " \
      "tree misses: %zu\ntransposition tree hit rate: %f\nbeta fail highs: "   \
      "%zu\nbeta fail highs per move: %f\nAverage depth scanned: %f\n"         \
      "Average boards scanned: %f\n",                                          \
      BOT, (DBG).moves_made, (DBG).transposition_tree_hits,                    \
      (DBG).transposition_tree_misses,                                         \
      (float)(DBG).transposition_tree_hits /                                   \
          (float)((DBG).transposition_tree_hits +                              \
                  (DBG).transposition_tree_misses),                            \
      (DBG).beta_fail_highs,                                                  \
      (float)(DBG).beta_fail_highs / (float)(DBG).moves_made,                  \
      (float)(DBG).total_depth_scanned / (float)(DBG).moves_made,              \
      (float)(DBG).scanned_boards / (float)(DBG).moves_made);

#else

#define dbg_mm()

#define dbg_tth()

#define dbg_ttm()

#define dbg_bfh()

#define dbg_tds()

#define dbg_sb()

#define dbg_display(bot)

#endif
