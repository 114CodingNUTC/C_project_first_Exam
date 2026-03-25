#include "config.h"

int ai_hard_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  return ai_easy_pick_move(game, out_row, out_col);
}
