#include "../include/config.h"
#include "../include/functions.h"

/*
 * ai_common.c - AI 共用戰術函式
 *
 * 提供三個難度都會重複使用的核心戰術判斷：
 * - 指定方向連線計數
 * - 立即致勝點搜尋
 * - 擋對手四連威脅
 * - 己方三連延伸到四連
 * - 近鄰是否有棋判斷
 * - 強制/戰術優先流程封裝
 */
int ai_count_line(const GomokuGame *game, int row, int col, int player, int dr,
                  int dc) {
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

static const int k_line_dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

static int find_line_length_range_move(const GomokuGame *game, int player,
                                       int min_len, int max_len, int *out_row,
                                       int *out_col) {
  int row;
  int col;
  int i;
  int len;
  GomokuGame temp;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      temp = *game;
      board_place_stone(&temp, player, row, col);

      for (i = 0; i < 4; i++) {
        len = 1 +
              ai_count_line(&temp, row, col, player, k_line_dirs[i][0],
                            k_line_dirs[i][1]) +
              ai_count_line(&temp, row, col, player, -k_line_dirs[i][0],
                            -k_line_dirs[i][1]);
        if (len >= min_len && len <= max_len) {
          *out_row = row;
          *out_col = col;
          return 1;
        }
      }
    }
  }

  return 0;
}

int ai_find_immediate_move(const GomokuGame *game, int player, int *out_row,
                           int *out_col) {
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

int ai_find_block_four_threat_move(const GomokuGame *game, int player,
                                   int *out_row, int *out_col) {
  return find_line_length_range_move(game, player, CFG_WIN_STREAK - 1,
                                     CFG_WIN_STREAK, out_row, out_col);
}

int ai_find_extend_three_move(const GomokuGame *game, int player, int *out_row,
                              int *out_col) {
  return find_line_length_range_move(game, player, CFG_WIN_STREAK - 1,
                                     CFG_WIN_STREAK - 1, out_row, out_col);
}

int ai_has_near_stone(const GomokuGame *game, int row, int col, int range) {
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

int ai_try_forced_or_tactical_move(const GomokuGame *game, int *out_row,
                                   int *out_col) {
  int opponent;

  opponent = (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

  if (ai_find_immediate_move(game, game->current_player, out_row, out_col))
    return 1;
  if (ai_find_immediate_move(game, opponent, out_row, out_col))
    return 1;
  if (ai_find_extend_three_move(game, game->current_player, out_row, out_col))
    return 1;
  if (ai_find_block_four_threat_move(game, opponent, out_row, out_col))
    return 1;

  return 0;
}
