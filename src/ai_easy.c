#include "../include/config.h"
#include "../include/functions.h"
#include <stdlib.h>
#include <time.h>

/**
 * @brief 簡單 AI：隨機挑選一個可落子位置。
 * @param game 當前遊戲狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表成功找到位置；0 代表無合法位置或參數錯誤。
 */
int ai_easy_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int row;
  int col;
  int count;
  int target;
  int moves[CFG_MAX_BOARD_SIZE * CFG_MAX_BOARD_SIZE][2];

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  /* Collect all empty positions */
  count = 0;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (game->board[row][col] == STONE_EMPTY) {
        moves[count][0] = row;
        moves[count][1] = col;
        count++;
      }
    }
  }

  if (count == 0)
    return 0;

  /* Pick random move from empty positions */
  target = rand() % count;
  *out_row = moves[target][0];
  *out_col = moves[target][1];
  return 1;
}
