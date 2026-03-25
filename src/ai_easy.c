#include "config.h"

int ai_easy_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int row;
  int col;

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (game->board[row][col] == STONE_EMPTY) {
        *out_row = row;
        *out_col = col;
        return 1;
      }
    }
  }

  return 0;
}
