#include "config.h"

static int clamp_board_size(int board_size) {
  if (board_size <= 0)
    return CFG_BOARD_SIZE_DEFAULT;
  if (board_size > CFG_MAX_BOARD_SIZE)
    return CFG_MAX_BOARD_SIZE;
  return board_size;
}

void board_init(GomokuGame *game, int board_size) {
  int row;
  int col;

  if (game == 0)
    return;

  game->board_size = clamp_board_size(board_size);
  for (row = 0; row < CFG_MAX_BOARD_SIZE; row++)
    for (col = 0; col < CFG_MAX_BOARD_SIZE; col++)
      game->board[row][col] = STONE_EMPTY;
}

int board_is_in_bounds(const GomokuGame *game, int row, int col) {
  if (game == 0)
    return 0;
  if (row < 0 || col < 0)
    return 0;
  if (row >= game->board_size || col >= game->board_size)
    return 0;
  return 1;
}

int board_is_empty(const GomokuGame *game, int row, int col) {
  if (!board_is_in_bounds(game, row, col))
    return 0;
  return game->board[row][col] == STONE_EMPTY;
}

int board_place_stone(GomokuGame *game, int player, int row, int col) {
  if (!board_is_empty(game, row, col))
    return 0;
  game->board[row][col] = player;
  return 1;
}
