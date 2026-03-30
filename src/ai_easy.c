#include "../include/config.h"
#include "../include/functions.h"
#include <stdlib.h>

static int has_adjacent_stone(const GomokuGame *game, int row, int col,
                              int range) {
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

static int find_immediate_win_for_player(const GomokuGame *game, int player,
                                         int *out_row, int *out_col) {
  int row;
  int col;
  int out_count;
  int out_line[CFG_WIN_STREAK][2];
  GomokuGame temp;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      temp = *game;
      board_place_stone(&temp, player, row, col);
      if (rules_check_win(&temp, row, col, player, out_line, &out_count)) {
        *out_row = row;
        *out_col = col;
        return 1;
      }
    }
  }

  return 0;
}

static int find_block_four_threat_move(const GomokuGame *game, int opponent,
                                       int *out_row, int *out_col) {
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

static int find_extend_three_move(const GomokuGame *game, int player,
                                  int *out_row, int *out_col) {
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
 * @brief 簡單 AI：優化随機選擇，優先靠近棋子或中心區域。
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

  if (find_immediate_win_for_player(game, game->current_player, out_row,
                                    out_col)) {
    return 1;
  }

  opponent = (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

  if (find_immediate_win_for_player(game, opponent, out_row, out_col))
    return 1;

  if (find_extend_three_move(game, game->current_player, out_row, out_col))
    return 1;

  if (find_block_four_threat_move(game, opponent, out_row, out_col))
    return 1;

  center = game->board_size / 2;

  ranked_count = 0;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      if (game->move_count > 0 &&
          !has_adjacent_stone(game, row, col, CFG_AI_EASY_NEAR_RANGE)) {
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
