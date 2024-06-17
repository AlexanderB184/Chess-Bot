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

extern int active_bot;
extern struct debug_info dbg[2];

#define dbg_header() int active_bot; struct debug_info dbg[2];

#define dbg_clr() \
  active_bot = 0; \
  memset(dbg, 0, sizoef(dbg));

#define dbg_mm() dbg[active_bot].moves_made++;

#define dbg_tth() dbg[active_bot].transposition_tree_hits++;

#define dbg_ttm() dbg[active_bot].transposition_tree_misses++;

#define dbg_bfh() dbg[active_bot].beta_fail_highs++;

#define dbg_tds() dbg[active_bot].total_depth_scanned++;

#define dbg_sb() dbg[active_bot].scanned_boards++;

#define dbg_display(bot)                                                      \
  printf(                                                                     \
      "bot: %d\nmoves made: %zu\ntransposition tree hits: %zu\ntransposition " \
      "tree misses: %zu\ntransposition tree hit rate: %f\nbeta fail highs: "   \
      "%zu\nbeta fail highs per move: %f\nAverage depth scanned: %f\n"       \
      "Average boards scanned: %f\n",                                         \
      bot, dbg[bot].moves_made, dbg[bot].transposition_tree_hits,             \
      dbg[bot].transposition_tree_misses,                                     \
      (float)dbg[bot].transposition_tree_hits /                               \
          (float)(dbg[bot].transposition_tree_hits +                          \
                  dbg[bot].transposition_tree_misses),                        \
      dbg->beta_fail_highs,                                                   \
      (float)dbg[bot].beta_fail_highs / (float)dbg[bot].moves_made,           \
      (float)dbg[bot].total_depth_scanned / (float)dbg[bot].moves_made,       \
      (float)dbg[bot].scanned_boards / (float)dbg[bot].moves_made);

#else

#define dbg_mm()

#define dbg_tth()

#define dbg_ttm()

#define dbg_bfh()

#define dbg_tds()

#define dbg_sb()

#define dbg_display(bot)

#endif
