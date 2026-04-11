#include "../include/config.h"
#include "../include/functions.h"
#include <stdlib.h>

/*
 * ai_easy.c - 簡單 AI 演算法說明
 *
 * 策略流程（由高到低優先級）：
 * 1) 先找自己立即致勝點。
 * 2) 再找對手立即致勝點並阻擋。
 * 3) 嘗試把己方三連延伸成四連（進攻壓力）。
 * 4) 阻擋對手把三連延伸成四連（防守底線）。
 * 5) 若無強制手，對合法空位做輕量評分：鄰近同色、鄰近敵色、中心偏好。
 * 6) 從前 K 個高分點隨機選擇，保留簡單難度的可預測性與變化性。
 */

/**
 * @brief 計算目標空位 8 鄰域內指定顏色棋子的數量。
 */
static int count_adjacent_color(const GomokuGame *game, int row, int col,
                                int player) {
  int dirs[8][2];
  int i;
  int nr;
  int nc;
  int count;

  dirs[0][0] = -1;
  dirs[0][1] = -1;
  dirs[1][0] = -1;
  dirs[1][1] = 0;
  dirs[2][0] = -1;
  dirs[2][1] = 1;
  dirs[3][0] = 0;
  dirs[3][1] = -1;
  dirs[4][0] = 0;
  dirs[4][1] = 1;
  dirs[5][0] = 1;
  dirs[5][1] = -1;
  dirs[6][0] = 1;
  dirs[6][1] = 0;
  dirs[7][0] = 1;
  dirs[7][1] = 1;

  count = 0;
  for (i = 0; i < 8; i++) {
    nr = row + dirs[i][0];
    nc = col + dirs[i][1];
    if (board_is_in_bounds(game, nr, nc) && game->board[nr][nc] == player)
      count++;
  }

  return count;
}

/**
 * @brief 簡單 AI：優化隨機選擇，優先靠近棋子或中心區域。
 * @param game 當前遊戲狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表成功找到位置；0 代表無合法位置或參數錯誤。
 */
int ai_easy_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int row;
  int col;
  int opponent;
  int center;
  int score;
  int target;
  int ranked_count;
  int i;
  int top_count;
  int ranked[CFG_MAX_BOARD_SIZE * CFG_MAX_BOARD_SIZE][3];

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  opponent = (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
  if (ai_try_forced_or_tactical_move(game, out_row, out_col))
    return 1;

  center = game->board_size / 2;

  ranked_count = 0;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      if (game->move_count > 0 &&
          !ai_has_near_stone(game, row, col, CFG_AI_EASY_NEAR_RANGE)) {
        continue;
      }

      score = count_adjacent_color(game, row, col, game->current_player) *
              CFG_AI_EASY_WEIGHT_SELF;
      score += count_adjacent_color(game, row, col, opponent) *
               CFG_AI_EASY_WEIGHT_OPP;
      score -= (abs(row - center) + abs(col - center));
      score += CFG_AI_EASY_CENTER_BONUS;

      i = ranked_count;
      while (i > 0 && ranked[i - 1][2] < score) {
        ranked[i][0] = ranked[i - 1][0];
        ranked[i][1] = ranked[i - 1][1];
        ranked[i][2] = ranked[i - 1][2];
        i--;
      }

      ranked[i][0] = row;
      ranked[i][1] = col;
      ranked[i][2] = score;
      ranked_count++;
    }
  }

  if (ranked_count == 0) {
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

  top_count = CFG_AI_EASY_TOP_K;
  if (top_count > ranked_count)
    top_count = ranked_count;

  target = rand() % top_count;
  *out_row = ranked[target][0];
  *out_col = ranked[target][1];
  return 1;
}
