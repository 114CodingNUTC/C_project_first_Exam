#include "../include/config.h"
#include "../include/functions.h"
#include <stdlib.h>

/**
 * @brief 模擬指定座標落子後是否可立即成線。
 * @param game 當前遊戲狀態。
 * @param player 目標玩家。
 * @param row 測試列索引。
 * @param col 測試行索引。
 * @param out_line 成線輸出暫存。
 * @return 1 代表可形成勝利；0 代表不可。
 */
static int test_position_win(const GomokuGame *game, int player, int row,
                             int col, int out_line[CFG_WIN_STREAK][2]) {
  int count;

  if (!board_is_empty(game, row, col))
    return 0;

  return rules_check_win(game, row, col, player, out_line, &count);
}

/**
 * @brief 尋找可立即致勝或阻擋的位置。
 * @param game 當前遊戲狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @param player 要檢查的玩家。
 * @return 1 代表找到關鍵位置；0 代表無。
 */
static int find_critical_move(const GomokuGame *game, int *out_row,
                              int *out_col, int player) {
  int row;
  int col;
  int out_line[CFG_WIN_STREAK][2];

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (test_position_win(game, player, row, col, out_line)) {
        *out_row = row;
        *out_col = col;
        return 1;
      }
    }
  }

  return 0;
}

/**
 * @brief 檢查位置附近是否有既有棋子。
 * @param game 當前遊戲狀態。
 * @param row 目標列索引。
 * @param col 目標行索引。
 * @param range 鄰近搜尋半徑。
 * @return 1 代表附近有棋子；0 代表沒有。
 */
static int is_near_stone(const GomokuGame *game, int row, int col, int range) {
  int r;
  int c;
  int r_start;
  int r_end;
  int c_start;
  int c_end;

  r_start = row - range;
  if (r_start < 0)
    r_start = 0;
  r_end = row + range;
  if (r_end >= game->board_size)
    r_end = game->board_size - 1;

  c_start = col - range;
  if (c_start < 0)
    c_start = 0;
  c_end = col + range;
  if (c_end >= game->board_size)
    c_end = game->board_size - 1;

  for (r = r_start; r <= r_end; r++) {
    for (c = c_start; c <= c_end; c++)
      if (game->board[r][c] != STONE_EMPTY)
        return 1;
  }

  return 0;
}

/**
 * @brief 中等 AI：防守優先，其次進攻，再選鄰近點。
 * @param game 當前遊戲狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表成功找到位置；0 代表無合法位置或參數錯誤。
 */
int ai_medium_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int row;
  int col;
  int opponent;
  int range;

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  opponent = (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

  /* Step 1: Check if opponent can win - DEFEND */
  if (find_critical_move(game, out_row, out_col, opponent))
    return 1;

  /* Step 2: Check if we can win - ATTACK */
  if (find_critical_move(game, out_row, out_col, game->current_player))
    return 1;

  /* Step 3: Pick position near existing stones */
  range = CFG_AI_MEDIUM_NEAR_RANGE;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (board_is_empty(game, row, col) &&
          is_near_stone(game, row, col, range)) {
        *out_row = row;
        *out_col = col;
        return 1;
      }
    }
  }

  /* Fallback to any empty position */
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (board_is_empty(game, row, col)) {
        *out_row = row;
        *out_col = col;
        return 1;
      }
    }
  }

  return 0;
}
