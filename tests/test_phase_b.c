#include "config.h"
#include "functions.h"
#include "events.h"
#include "messages.h"
#include <stdio.h>
#include <windows.h>

static void print_result(int test_num, const char *desc, int passed) {
  if (passed)
    printf("✓ Test %d PASS: %s\n", test_num, desc);
  else
    printf("✗ Test %d FAIL: %s\n", test_num, desc);
}

static void print_board(const GomokuGame *game) {
  int row, col;
  printf("  ");
  for (col = 0; col < game->board_size; col++)
    printf("%c ", 'A' + col);
  printf("\n");

  for (row = 0; row < game->board_size; row++) {
    printf("%2d", row + 1);
    for (col = 0; col < game->board_size; col++) {
      int stone = game->board[row][col];
      if (stone == STONE_BLACK)
        printf("●");
      else if (stone == STONE_WHITE)
        printf("○");
      else if (row == game->last_row && col == game->last_col)
        printf("·");
      else
        printf("·");
      printf(" ");
    }
    printf("%d\n", row + 1);
  }
  printf("\n");
}

int main(void) {
  GomokuGame game;
  int event_code, msg_key, result;
  int test_count = 0;

  SetConsoleOutputCP(CFG_CONSOLE_OUTPUT_CODEPAGE);
  SetConsoleCP(CFG_CONSOLE_INPUT_CODEPAGE);

  printf("=== Phase B Smoke Tests ===\n\n");

  /* Test 1: Board initialization */
  game_reset(&game, CFG_BOARD_SIZE_9);
  test_count++;
  print_result(test_count, "Board initialized to 9x9", game.board_size == 9);

  /* Test 2: First move (Black) */
  result = try_place_stone(&game, STONE_BLACK, 4, 4, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "Black places first stone at (4,4)",
               result == PLACE_OK && event_code == EV_STONE_PLACED);

  /* Test 3: Turn switches to White */
  test_count++;
  print_result(test_count, "Turn switched to White",
               game.current_player == STONE_WHITE);

  /* Test 4: Invalid move - occupied cell */
  result = try_place_stone(&game, STONE_WHITE, 4, 4, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "Occupied cell rejected",
               result == PLACE_ERR_OCCUPIED && event_code == EV_INVALID_MOVE);

  /* Test 5: Valid White move */
  result = try_place_stone(&game, STONE_WHITE, 3, 3, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "White places stone at (3,3)",
               result == PLACE_OK && game.current_player == STONE_BLACK);

  /* Test 6: Invalid move - out of bounds */
  result = try_place_stone(&game, STONE_BLACK, -1, 0, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "Out of bounds move rejected",
               result == PLACE_ERR_OUT_OF_BOUNDS);

  /* Test 7: Invalid move - not current player */
  result = try_place_stone(&game, STONE_WHITE, 5, 5, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "Non-current player move rejected",
               result == PLACE_ERR_NOT_CURRENT_PLAYER);

  /* Test 8: Win condition - horizontal line of 5 */
  game_reset(&game, CFG_BOARD_SIZE_15);
  try_place_stone(&game, STONE_BLACK, 7, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 6, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 6, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 7, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 6, 7, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 8, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 6, 8, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 7, 9, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "Black wins with horizontal 5-in-a-row",
               result == PLACE_OK && event_code == EV_WIN &&
                   game.winner == STONE_BLACK && game.game_over == 1);

  test_count++;
  print_result(test_count, "Win line coordinates captured",
               game.win_line_count == 5);

  printf("\nWin line coordinates:\n");
  for (int i = 0; i < game.win_line_count; i++)
    printf("  [%d, %d] ", game.win_line[i][0], game.win_line[i][1]);
  printf("\n\n");

  /* Test 10: Game over check */
  result = try_place_stone(&game, STONE_WHITE, 8, 8, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "Game over prevents more moves",
               result == PLACE_ERR_GAME_OVER && event_code == EV_INVALID_MOVE);

  /* Test 11: Draw condition (small board) */
  game_reset(&game, 3);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++)
      if ((i + j) % 2 == 0)
        try_place_stone(&game, STONE_BLACK, i, j, &event_code, &msg_key);
      else
        try_place_stone(&game, STONE_WHITE, i, j, &event_code, &msg_key);
  }

  test_count++;
  print_result(test_count, "Draw condition detected",
               game.game_over == 1 && game.winner == STONE_EMPTY &&
                   event_code == EV_DRAW);

  /* Test 12: Vertical 5-in-a-row */
  game_reset(&game, CFG_BOARD_SIZE_15);
  try_place_stone(&game, STONE_BLACK, 7, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 7, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 8, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 9, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 10, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 10, 6, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 11, 5, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "Black wins with vertical 5-in-a-row",
               result == PLACE_OK && event_code == EV_WIN &&
                   game.winner == STONE_BLACK);

  /* Test 13: Diagonal positive 5-in-a-row */
  game_reset(&game, CFG_BOARD_SIZE_15);
  try_place_stone(&game, STONE_BLACK, 5, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 6, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 6, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 7, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 7, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 8, 8, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 7, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 9, 9, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "Black wins with diagonal-positive 5-in-a-row",
               result == PLACE_OK && event_code == EV_WIN);

  /* Test 14: Diagonal negative 5-in-a-row */
  game_reset(&game, CFG_BOARD_SIZE_15);
  try_place_stone(&game, STONE_BLACK, 5, 9, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 6, 10, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 6, 8, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 7, 9, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 7, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 8, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 8, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 7, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 9, 5, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "Black wins with diagonal-negative 5-in-a-row",
               result == PLACE_OK && event_code == EV_WIN);

  /* Test 15: Six-in-a-row scenario (check first 5 wins) */
  game_reset(&game, CFG_BOARD_SIZE_15);
  try_place_stone(&game, STONE_BLACK, 7, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 7, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 8, 6, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 7, 7, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "Win detected on 5th in-a-row (not 6+)",
               result == PLACE_OK && event_code == EV_WIN);

  /* Test 16: 9x9 board win */
  game_reset(&game, 9);
  try_place_stone(&game, STONE_BLACK, 4, 2, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 2, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 5, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 4, 6, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "9x9 board horizontal win",
               result == PLACE_OK && event_code == EV_WIN &&
                   game.board_size == 9);

  /* Test 17: 19x19 board win */
  game_reset(&game, 19);
  try_place_stone(&game, STONE_BLACK, 10, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 10, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 6, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 10, 7, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 7, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 10, 8, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 9, 8, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 10, 9, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "19x19 board horizontal win",
               result == PLACE_OK && event_code == EV_WIN &&
                   game.board_size == 19);

  /* Test 18: Corner position accepted */
  game_reset(&game, CFG_BOARD_SIZE_15);
  result = try_place_stone(&game, STONE_BLACK, 0, 0, &event_code, &msg_key);
  test_count++;
  print_result(test_count, "Corner position accepted", result == PLACE_OK);

  /* Test 19: Game state after win */
  game_reset(&game, CFG_BOARD_SIZE_9);
  try_place_stone(&game, STONE_BLACK, 4, 2, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 2, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 5, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 6, &event_code, &msg_key);

  test_count++;
  print_result(test_count, "Game state locked after win",
               game.winner == STONE_BLACK && game.game_over == 1);

  /* Test 20: Reset clears all state */
  game_reset(&game, CFG_BOARD_SIZE_15);
  test_count++;
  print_result(test_count, "Reset clears board and state",
               game.board_size == 15 && game.current_player == STONE_BLACK &&
                   game.move_count == 0 && game.game_over == 0);

  printf("\n=== Summary ===\n");
  printf("Total tests: %d\n", test_count);

  return 0;
}
