#include "../include/config.h"

static int count_direction(const GomokuGame *game, int row, int col, int dr,
                           int dc, int player, int line[CFG_WIN_STREAK][2],
                           int *index, int write_forward) {
  int r;
  int c;
  int count;

  count = 0;
  r = row + dr;
  c = col + dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    if (*index < CFG_WIN_STREAK && write_forward) {
      line[*index][0] = r;
      line[*index][1] = c;
      (*index)++;
    }
    count++;
    r += dr;
    c += dc;
  }
  return count;
}

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
    back = count_direction(game, row, col, -dr, -dc, player, out_line,
                           out_count, 0);
    forward =
        count_direction(game, row, col, dr, dc, player, out_line, out_count, 0);
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

int rules_is_draw(const GomokuGame *game) {
  if (game == 0)
    return 0;
  return game->move_count >= game->board_size * game->board_size;
}
