#include "../include/config.h"
#include "../include/functions.h"
#include <stdlib.h>

/*
 * ai_medium.c - 中等 AI 演算法說明
 *
 * 核心是「防守優先 + 形狀評分」：
 * 1) 立即致勝/立即防守（強制手）。
 * 2) 尋找可把己方三連延伸到四連的攻勢。
 * 3) 尋找可阻止對手形成四連威脅的防守手。
 * 4) 其餘候選點依威脅分數排序：
 *    - 攻擊形狀分（己方） * 攻擊權重
 *    - 防守形狀分（對手） * 防守權重
 *    - 中心偏好懲罰（越偏中心越佳）
 * 5) 先在近鄰範圍找最佳，再擴張範圍一次。
 */

/**
 * @brief 計算某方向的連線長度與兩端開口數，用於棋形評分。
 */
static void get_line_info(const GomokuGame *game, int row, int col, int player,
                          int dr, int dc, int *out_len, int *out_open_ends) {
  int len;
  int open_ends;
  int r;
  int c;

  len = 1;
  open_ends = 0;

  r = row + dr;
  c = col + dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    len++;
    r += dr;
    c += dc;
  }
  if (board_is_in_bounds(game, r, c) && game->board[r][c] == STONE_EMPTY)
    open_ends++;

  r = row - dr;
  c = col - dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    len++;
    r -= dr;
    c -= dc;
  }
  if (board_is_in_bounds(game, r, c) && game->board[r][c] == STONE_EMPTY)
    open_ends++;

  *out_len = len;
  *out_open_ends = open_ends;
}

/**
 * @brief 估算單點棋形價值（連線長度 + 開口數）。
 */
static int eval_shape_score(const GomokuGame *game, int row, int col,
                            int player) {
  int dirs[4][2];
  int i;
  int len;
  int open_ends;
  int total;

  dirs[0][0] = 0;
  dirs[0][1] = 1;
  dirs[1][0] = 1;
  dirs[1][1] = 0;
  dirs[2][0] = 1;
  dirs[2][1] = 1;
  dirs[3][0] = 1;
  dirs[3][1] = -1;

  total = 0;
  for (i = 0; i < 4; i++) {
    get_line_info(game, row, col, player, dirs[i][0], dirs[i][1], &len,
                  &open_ends);

    if (len >= CFG_WIN_STREAK) {
      total += CFG_AI_EVAL_WIN;
    } else if (len == 4) {
      total += CFG_AI_EVAL_FOUR;
      total += open_ends * CFG_AI_MEDIUM_WEIGHT_OPEN_END * CFG_AI_EVAL_TWO;
    } else if (len == 3) {
      total += CFG_AI_EVAL_THREE;
      total += open_ends * CFG_AI_MEDIUM_WEIGHT_OPEN_END * CFG_AI_EVAL_ONE;
    } else if (len == 2) {
      total += CFG_AI_EVAL_TWO;
      total += open_ends * CFG_AI_MEDIUM_WEIGHT_OPEN_END;
    }
  }

  return total;
}

/**
 * @brief 計算綜合威脅值：攻擊分、阻擋分與中心偏好。
 */
static int eval_threat_strength(const GomokuGame *game, int row, int col,
                                int player) {
  int opponent;
  int center;
  int score;

  if (!board_is_empty(game, row, col))
    return CFG_AI_SCORE_NEG_INF;

  opponent = (player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
  center = game->board_size / 2;

  score =
      eval_shape_score(game, row, col, player) * CFG_AI_MEDIUM_WEIGHT_ATTACK;
  score +=
      eval_shape_score(game, row, col, opponent) * CFG_AI_MEDIUM_WEIGHT_BLOCK;
  score -= (abs(row - center) + abs(col - center));

  return score;
}

/**
 * @brief 中等 AI：防守優先，其次進攻，評估威脅強度。
 * @param game 當前遊戲狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表成功找到位置；0 代表無合法位置或參數錯誤。
 */
int ai_medium_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int row;
  int col;
  int range;
  int best_threat;
  int threat;
  int best_row;
  int best_col;

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  if (ai_try_forced_or_tactical_move(game, out_row, out_col))
    return 1;

  /* Step 5: Pick position with highest threat strength near existing stones */
  best_threat = -1;
  best_row = -1;
  best_col = -1;
  range = CFG_AI_MEDIUM_NEAR_RANGE;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (board_is_empty(game, row, col) &&
          ai_has_near_stone(game, row, col, range)) {
        threat = eval_threat_strength(game, row, col, game->current_player);
        if (threat > best_threat) {
          best_threat = threat;
          best_row = row;
          best_col = col;
        }
      }
    }
  }

  if (best_row != -1) {
    *out_row = best_row;
    *out_col = best_col;
    return 1;
  }

  /* Step 6: Expand search zone with threat evaluation */
  range = CFG_AI_MEDIUM_NEAR_RANGE + CFG_AI_MEDIUM_EXPAND_RANGE_ADD;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (board_is_empty(game, row, col) &&
          ai_has_near_stone(game, row, col, range)) {
        threat = eval_threat_strength(game, row, col, game->current_player);
        if (threat > best_threat) {
          best_threat = threat;
          best_row = row;
          best_col = col;
        }
      }
    }
  }

  if (best_row != -1) {
    *out_row = best_row;
    *out_col = best_col;
    return 1;
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
