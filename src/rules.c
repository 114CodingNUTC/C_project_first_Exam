#include "../include/config.h"
#include "../include/functions.h"

/**
 * @brief 沿指定方向計算連續同色棋子數。
 * @param game 遊戲狀態物件。
 * @param row 起點列索引。
 * @param col 起點行索引。
 * @param dr 列方向增量。
 * @param dc 行方向增量。
 * @param player 目標棋子顏色。
 * @param line 成線輸出陣列。
 * @param index 成線寫入索引指標。
 * @param write_forward 是否寫入 line。
 * @return 該方向上的連續棋子數。
 */
static int count_direction(const GomokuGame *game, int row, int col, int dr,
                           int dc, int player) {
  int r;
  int c;
  int count;

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
 * @brief 檢查最後一手是否形成連五。
 * @param game 遊戲狀態物件。
 * @param row 最後落子列索引。
 * @param col 最後落子行索引。
 * @param player 最後落子方。
 * @param out_line 輸出連線五子座標。
 * @param out_count 輸出座標數量。
 * @return 1 代表有勝利；0 代表尚未勝利。
 */
int rules_check_win(const GomokuGame *game, int row, int col, int player,
                    int out_line[CFG_WIN_STREAK][2], int *out_count) {
  const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
  int i;

  if (game == 0 || out_count == 0)
    return 0;

  for (i = 0; i < 4; i++) {
    int dr;
    int dc;
    int back;
    int forward;
    int total;
    int start_r;
    int start_c;

    dr = dirs[i][0];
    dc = dirs[i][1];
    back = count_direction(game, row, col, -dr, -dc, player);
    forward = count_direction(game, row, col, dr, dc, player);
    total = 1 + back + forward;

    if (total >= CFG_WIN_STREAK) {
      int k;
      start_r = row - back * dr;
      start_c = col - back * dc;
      for (k = 0; k < CFG_WIN_STREAK; k++) {
        out_line[k][0] = start_r + k * dr;
        out_line[k][1] = start_c + k * dc;
      }
      *out_count = CFG_WIN_STREAK;
      return 1;
    }
  }

  *out_count = 0;
  return 0;
}

/**
 * @brief 檢查棋局是否為和局。
 * @param game 遊戲狀態物件。
 * @return 1 代表和局；0 代表尚未和局。
 */
int rules_is_draw(const GomokuGame *game) {
  if (game == 0)
    return 0;
  return game->move_count >= game->board_size * game->board_size;
}
