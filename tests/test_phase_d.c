#include "../include/config.h"
#include "../include/functions.h"
#include <stdio.h>

static int g_total = 0;
static int g_pass = 0;

static void record_result(const char *name, int ok) {
  g_total++;
  if (ok) {
    g_pass++;
    printf("PASS: %s\n", name);
  } else {
    printf("FAIL: %s\n", name);
  }
}

static void fill_board_full(GomokuGame *game, int board_size) {
  int row;
  int col;

  board_init(game, board_size);
  for (row = 0; row < board_size; row++) {
    for (col = 0; col < board_size; col++) {
      board_place_stone(
          game, ((row + col) % 2 == 0) ? STONE_BLACK : STONE_WHITE, row, col);
    }
  }
}

static void test_ai_easy(void) {
  GomokuGame game;
  int row;
  int col;
  int ok;

  printf("\n=== AI Easy Tests ===\n");

  board_init(&game, CFG_BOARD_SIZE_9);
  game.current_player = STONE_BLACK;
  ok = ai_easy_pick_move(&game, &row, &col) &&
       board_is_in_bounds(&game, row, col) && board_is_empty(&game, row, col);
  record_result("easy picks valid move on empty board", ok);

  board_place_stone(&game, STONE_BLACK, 4, 4);
  board_place_stone(&game, STONE_WHITE, 4, 5);
  ok = ai_easy_pick_move(&game, &row, &col) && board_is_empty(&game, row, col);
  record_result("easy picks empty move on partial board", ok);

  fill_board_full(&game, CFG_BOARD_SIZE_9);
  game.current_player = STONE_BLACK;
  ok = !ai_easy_pick_move(&game, &row, &col);
  record_result("easy returns false on full board", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 0);
  board_place_stone(&game, STONE_WHITE, 4, 1);
  board_place_stone(&game, STONE_WHITE, 4, 2);
  board_place_stone(&game, STONE_WHITE, 4, 3);
  game.current_player = STONE_BLACK;
  ok = ai_easy_pick_move(&game, &row, &col) && row == 4 && col == 4;
  record_result("easy blocks opponent three-to-four threat", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 1);
  board_place_stone(&game, STONE_BLACK, 4, 2);
  board_place_stone(&game, STONE_BLACK, 4, 3);
  board_place_stone(&game, STONE_WHITE, 5, 5);
  game.current_player = STONE_BLACK;
  ok = ai_easy_pick_move(&game, &row, &col) &&
       (row == 4 && col == 0 || row == 4 && col == 4);
  record_result("easy extends own three to four", ok);
}

static void test_ai_medium(void) {
  GomokuGame game;
  int row;
  int col;
  int ok;

  printf("\n=== AI Medium Tests ===\n");

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_WHITE, 4, 1);
  board_place_stone(&game, STONE_WHITE, 4, 2);
  board_place_stone(&game, STONE_WHITE, 4, 3);
  board_place_stone(&game, STONE_WHITE, 4, 4);
  board_place_stone(&game, STONE_BLACK, 4, 5);
  game.current_player = STONE_BLACK;
  ok = ai_medium_pick_move(&game, &row, &col) && row == 4 && col == 0;
  record_result("medium blocks opponent immediate win", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 1);
  board_place_stone(&game, STONE_BLACK, 4, 2);
  board_place_stone(&game, STONE_BLACK, 4, 3);
  board_place_stone(&game, STONE_BLACK, 4, 4);
  board_place_stone(&game, STONE_WHITE, 4, 0);
  game.current_player = STONE_BLACK;
  ok = ai_medium_pick_move(&game, &row, &col) && row == 4 && col == 5;
  record_result("medium takes immediate winning move", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 4);
  game.current_player = STONE_WHITE;
  ok =
      ai_medium_pick_move(&game, &row, &col) && board_is_empty(&game, row, col);
  record_result("medium returns legal move on normal state", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 0);
  board_place_stone(&game, STONE_WHITE, 4, 1);
  board_place_stone(&game, STONE_WHITE, 4, 2);
  board_place_stone(&game, STONE_WHITE, 4, 3);
  game.current_player = STONE_BLACK;
  ok = ai_medium_pick_move(&game, &row, &col) && row == 4 && col == 4;
  record_result("medium blocks opponent three-to-four threat", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 1);
  board_place_stone(&game, STONE_BLACK, 4, 2);
  board_place_stone(&game, STONE_BLACK, 4, 3);
  board_place_stone(&game, STONE_WHITE, 5, 5);
  game.current_player = STONE_BLACK;
  ok = ai_medium_pick_move(&game, &row, &col) &&
       (row == 4 && col == 0 || row == 4 && col == 4);
  record_result("medium extends own three to four", ok);
}

static void test_ai_hard(void) {
  GomokuGame game;
  int row;
  int col;
  int ok;

  printf("\n=== AI Hard Tests ===\n");

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 1);
  board_place_stone(&game, STONE_BLACK, 4, 2);
  board_place_stone(&game, STONE_BLACK, 4, 3);
  board_place_stone(&game, STONE_BLACK, 4, 4);
  board_place_stone(&game, STONE_WHITE, 4, 0);
  game.current_player = STONE_BLACK;
  ok = ai_hard_pick_move(&game, &row, &col) && row == 4 && col == 5;
  record_result("hard finds immediate winning move", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_WHITE, 4, 1);
  board_place_stone(&game, STONE_WHITE, 4, 2);
  board_place_stone(&game, STONE_WHITE, 4, 3);
  board_place_stone(&game, STONE_WHITE, 4, 4);
  board_place_stone(&game, STONE_BLACK, 4, 5);
  game.current_player = STONE_BLACK;
  ok = ai_hard_pick_move(&game, &row, &col) && row == 4 && col == 0;
  record_result("hard blocks opponent immediate win", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 4);
  game.current_player = STONE_WHITE;
  ok = ai_hard_pick_move(&game, &row, &col) && board_is_empty(&game, row, col);
  record_result("hard returns legal move on normal state", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 0);
  board_place_stone(&game, STONE_WHITE, 4, 1);
  board_place_stone(&game, STONE_WHITE, 4, 2);
  board_place_stone(&game, STONE_WHITE, 4, 3);
  game.current_player = STONE_BLACK;
  ok = ai_hard_pick_move(&game, &row, &col) && row == 4 && col == 4;
  record_result("hard blocks opponent three-to-four threat", ok);

  board_init(&game, CFG_BOARD_SIZE_9);
  board_place_stone(&game, STONE_BLACK, 4, 1);
  board_place_stone(&game, STONE_BLACK, 4, 2);
  board_place_stone(&game, STONE_BLACK, 4, 3);
  board_place_stone(&game, STONE_WHITE, 5, 5);
  game.current_player = STONE_BLACK;
  ok = ai_hard_pick_move(&game, &row, &col) &&
       (row == 4 && col == 0 || row == 4 && col == 4);
  record_result("hard extends own three to four", ok);
}

int main(void) {
  printf("=== Phase D Independent Test File ===\n");

  test_ai_easy();
  test_ai_medium();
  test_ai_hard();

  printf("\n=== Phase D Summary ===\n");
  printf("Passed: %d/%d\n", g_pass, g_total);

  return (g_pass == g_total) ? 0 : 1;
}
