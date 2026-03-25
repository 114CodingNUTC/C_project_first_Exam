#include "config.h"
#include "events.h"
#include "messages.h"
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

static void test_state_and_event_constants(void) {
  record_result("state constants are distinct",
                GS_MAIN_MENU != GS_IN_GAME && GS_IN_GAME != GS_PAUSED &&
                    GS_PAUSED != GS_CONFIRM_EXIT);

  record_result("phase E event constants are valid",
                EV_PAUSED > EV_INVALID_MOVE && EV_RESUMED > EV_PAUSED &&
                    EV_RESTARTED > EV_RESUMED && EV_EXIT_APP > EV_EXIT_CONFIRM);

  record_result("game loop return constants are valid",
                GAME_LOOP_BACK_TO_MENU == 0 && GAME_LOOP_EXIT_APP == 1);
}

static void test_round_flow_event_sequence(void) {
  GomokuGame game;
  int event_code;
  int message_key;
  int result;

  game_reset(&game, CFG_BOARD_SIZE_9);

  result = try_place_stone(&game, STONE_BLACK, 4, 2, &event_code, &message_key);
  record_result("round flow: first move accepted",
                result == PLACE_OK && event_code == EV_STONE_PLACED &&
                    game.current_player == STONE_WHITE);

  result = try_place_stone(&game, STONE_WHITE, 3, 2, &event_code, &message_key);
  record_result("round flow: second move accepted",
                result == PLACE_OK && event_code == EV_STONE_PLACED &&
                    game.current_player == STONE_BLACK);

  try_place_stone(&game, STONE_BLACK, 4, 3, &event_code, &message_key);
  try_place_stone(&game, STONE_WHITE, 3, 3, &event_code, &message_key);
  try_place_stone(&game, STONE_BLACK, 4, 4, &event_code, &message_key);
  try_place_stone(&game, STONE_WHITE, 3, 4, &event_code, &message_key);
  try_place_stone(&game, STONE_BLACK, 4, 5, &event_code, &message_key);
  try_place_stone(&game, STONE_WHITE, 3, 5, &event_code, &message_key);

  result = try_place_stone(&game, STONE_BLACK, 4, 6, &event_code, &message_key);
  record_result("round flow: win event emitted", result == PLACE_OK &&
                                                     event_code == EV_WIN &&
                                                     game.game_over == 1);
}

int main(void) {
  printf("=== Phase E Integration Test ===\n");

  test_state_and_event_constants();
  test_round_flow_event_sequence();

  printf("\n=== Phase E Summary ===\n");
  printf("Passed: %d/%d\n", g_pass, g_total);

  return (g_pass == g_total) ? 0 : 1;
}
