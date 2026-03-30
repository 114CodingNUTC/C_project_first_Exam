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

static int count_line(const GomokuGame *game, int row, int col, int player,
                      int dr, int dc) {
  int count;
  int r;
  int c;

  count = 0;
  r = row + dr;
  c = col + dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    count++;
    r += dr;
    c += dc;
  }

  return count;
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

static int find_block_four_threat_move(const GomokuGame *game, int *out_row,
                                       int *out_col, int opponent) {
  int row;
  int col;
  int i;
  int len;
  int dirs[4][2];
  GomokuGame temp;

  dirs[0][0] = 0;
  dirs[0][1] = 1;
  dirs[1][0] = 1;
  dirs[1][1] = 0;
  dirs[2][0] = 1;
  dirs[2][1] = 1;
  dirs[3][0] = 1;
  dirs[3][1] = -1;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      temp = *game;
      board_place_stone(&temp, opponent, row, col);

      for (i = 0; i < 4; i++) {
        len = 1 +
              count_line(&temp, row, col, opponent, dirs[i][0], dirs[i][1]) +
              count_line(&temp, row, col, opponent, -dirs[i][0], -dirs[i][1]);
        if (len >= CFG_WIN_STREAK - 1) {
          *out_row = row;
          *out_col = col;
          return 1;
        }
      }
    }
  }

  return 0;
}

static int find_extend_three_move(const GomokuGame *game, int *out_row,
                                  int *out_col, int player) {
  int row;
  int col;
  int i;
  int len;
  int dirs[4][2];
  GomokuGame temp;

  dirs[0][0] = 0;
  dirs[0][1] = 1;
  dirs[1][0] = 1;
  dirs[1][1] = 0;
  dirs[2][0] = 1;
  dirs[2][1] = 1;
  dirs[3][0] = 1;
  dirs[3][1] = -1;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      temp = *game;
      board_place_stone(&temp, player, row, col);

      for (i = 0; i < 4; i++) {
        len = 1 + count_line(&temp, row, col, player, dirs[i][0], dirs[i][1]) +
              count_line(&temp, row, col, player, -dirs[i][0], -dirs[i][1]);
        if (len == CFG_WIN_STREAK - 1) {
          *out_row = row;
          *out_col = col;
          return 1;
        }
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
  int opponent;
  int range;
  int best_threat;
  int threat;
  int best_row;
  int best_col;

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  opponent = (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

  /* Step 1: Check if we can win immediately */
  if (find_critical_move(game, out_row, out_col, game->current_player))
    return 1;

  /* Step 2: Block opponent immediate win */
  if (find_critical_move(game, out_row, out_col, opponent))
    return 1;

  /* Step 3: Extend our three into four when possible */
  if (find_extend_three_move(game, out_row, out_col, game->current_player))
    return 1;

  /* Step 4: Block opponent from turning three into four */
  if (find_block_four_threat_move(game, out_row, out_col, opponent))
    return 1;

  /* Step 5: Pick position with highest threat strength near existing stones */
  best_threat = -1;
  best_row = -1;
  best_col = -1;
  range = CFG_AI_MEDIUM_NEAR_RANGE;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (board_is_empty(game, row, col) &&
          is_near_stone(game, row, col, range)) {
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
          is_near_stone(game, row, col, range)) {
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
