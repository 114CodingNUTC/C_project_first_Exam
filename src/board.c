#include "../include/config.h"
#include "../include/functions.h"

/**
 * @brief 將棋盤尺寸限制在可接受範圍。
 * @param board_size 使用者輸入的棋盤邊長。
 * @return 經過修正後的合法棋盤邊長。
 */
static int clamp_board_size(int board_size) {
  if (board_size <= 0)
    return CFG_BOARD_SIZE_DEFAULT;
  if (board_size > CFG_MAX_BOARD_SIZE)
    return CFG_MAX_BOARD_SIZE;
  return board_size;
}

/**
 * @brief 初始化棋盤內容與尺寸。
 * @param game 遊戲狀態物件。
 * @param board_size 目標棋盤邊長。
 */
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

/**
 * @brief 檢查座標是否位於棋盤範圍內。
 * @param game 遊戲狀態物件。
 * @param row 目標列索引。
 * @param col 目標行索引。
 * @return 1 代表在範圍內；0 代表超界或參數無效。
 */
int board_is_in_bounds(const GomokuGame *game, int row, int col) {
  if (game == 0)
    return 0;
  if (row < 0 || col < 0)
    return 0;
  if (row >= game->board_size || col >= game->board_size)
    return 0;
  return 1;
}

/**
 * @brief 檢查指定位置是否為空格。
 * @param game 遊戲狀態物件。
 * @param row 目標列索引。
 * @param col 目標行索引。
 * @return 1 代表可落子；0 代表不可落子。
 */
int board_is_empty(const GomokuGame *game, int row, int col) {
  if (!board_is_in_bounds(game, row, col))
    return 0;
  return game->board[row][col] == STONE_EMPTY;
}

/**
 * @brief 在指定位置放置棋子。
 * @param game 遊戲狀態物件。
 * @param player 棋子顏色。
 * @param row 目標列索引。
 * @param col 目標行索引。
 * @return 1 代表成功放置；0 代表失敗。
 */
int board_place_stone(GomokuGame *game, int player, int row, int col) {
  if (!board_is_empty(game, row, col))
    return 0;
  game->board[row][col] = player;
  return 1;
}
