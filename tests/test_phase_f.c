#include "config.h"
#include "events.h"
#include "functions.h"
#include "messages.h"
#include <stdio.h>
#include <string.h>

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

static void test_input_matrix(void) {
  int board_sizes[3] = {CFG_BOARD_SIZE_9, CFG_BOARD_SIZE_15, CFG_BOARD_SIZE_19};
  const char *inputs[3] = {"a1", "1a", "a,1"};
  int i;
  int j;
  int row;
  int col;
  int ok;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      ok = input_parse_move(inputs[j], board_sizes[i], &row, &col) &&
           row == 0 && col == 0;
      record_result("matrix input parse (a1/1a/a,1 x board size)", ok);
    }
  }
}

static int ai_pick_legal(const GomokuGame *game,
                         int (*pick)(const GomokuGame *, int *, int *)) {
  int row;
  int col;
  if (!pick(game, &row, &col))
    return 0;
  return board_is_in_bounds(game, row, col) && board_is_empty(game, row, col);
}

static void test_ai_matrix(void) {
  int board_sizes[3] = {CFG_BOARD_SIZE_9, CFG_BOARD_SIZE_15, CFG_BOARD_SIZE_19};
  int i;
  GomokuGame game;

  for (i = 0; i < 3; i++) {
    game_reset(&game, board_sizes[i]);
    game.current_player = STONE_WHITE;
    record_result("easy ai legal move on size matrix",
                  ai_pick_legal(&game, ai_easy_pick_move));
    record_result("medium ai legal move on size matrix",
                  ai_pick_legal(&game, ai_medium_pick_move));
    record_result("hard ai legal move on size matrix",
                  ai_pick_legal(&game, ai_hard_pick_move));
  }
}

static void test_smoke_win_draw_invalid(void) {
  GomokuGame game;
  int event_code;
  int msg_key;
  int result;
  int row;
  int col;

  game_reset(&game, CFG_BOARD_SIZE_9);
  try_place_stone(&game, STONE_BLACK, 4, 1, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 1, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 2, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 2, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 3, &event_code, &msg_key);
  try_place_stone(&game, STONE_BLACK, 4, 4, &event_code, &msg_key);
  try_place_stone(&game, STONE_WHITE, 3, 4, &event_code, &msg_key);
  result = try_place_stone(&game, STONE_BLACK, 4, 5, &event_code, &msg_key);
  record_result("smoke win event", result == PLACE_OK && event_code == EV_WIN);

  game_reset(&game, CFG_BOARD_SIZE_9);
  result = try_place_stone(&game, STONE_BLACK, -1, 0, &event_code, &msg_key);
  record_result("smoke invalid out-of-bounds",
                result == PLACE_ERR_OUT_OF_BOUNDS &&
                    event_code == EV_INVALID_MOVE);

  game_reset(&game, CFG_BOARD_SIZE_9);
  game.current_player = STONE_WHITE;
  result = ai_easy_pick_move(&game, &row, &col);
  record_result("smoke ai returns coordinate", result == 1);

  game_reset(&game, 3);
  for (row = 0; row < 3; row++) {
    for (col = 0; col < 3; col++) {
      if ((row + col) % 2 == 0) {
        result = try_place_stone(&game, STONE_BLACK, row, col, &event_code,
                                 &msg_key);
      } else {
        result = try_place_stone(&game, STONE_WHITE, row, col, &event_code,
                                 &msg_key);
      }
      if (result != PLACE_OK)
        break;
    }
  }
  record_result("smoke draw event",
                result == PLACE_OK && event_code == EV_DRAW);
}

static void test_event_and_message_centers(void) {
  int events[] = {EV_READY,        EV_ROUND_START,  EV_STONE_PLACED,
                  EV_SURRENDER,    EV_WIN,          EV_DRAW,
                  EV_INVALID_MOVE, EV_PAUSED,       EV_RESUMED,
                  EV_RESTARTED,    EV_BACK_TO_MENU, EV_EXIT_CONFIRM,
                  EV_EXIT_CANCEL,  EV_EXIT_APP,     EV_ERROR};
  int keys[] = {MSG_WELCOME,
                MSG_READY,
                MSG_TURN_BLACK,
                MSG_TURN_WHITE,
                MSG_WIN_BLACK,
                MSG_WIN_WHITE,
                MSG_DRAW,
                MSG_SURRENDER_BLACK,
                MSG_SURRENDER_WHITE,
                MSG_INVALID_INPUT,
                MSG_OUT_OF_BOUNDS,
                MSG_CELL_OCCUPIED,
                MSG_NOT_YOUR_TURN,
                MSG_GAME_OVER,
                MSG_INTERNAL_ERROR,
                MSG_MENU_MODE,
                MSG_MENU_BOARD,
                MSG_MODE_1V1,
                MSG_MODE_1V_AI_EASY,
                MSG_MODE_1V_AI_MEDIUM,
                MSG_MODE_1V_AI_HARD,
                MSG_BOARD_9X9,
                MSG_BOARD_15X15,
                MSG_BOARD_19X19,
                MSG_INPUT_HINT,
                MSG_MOVE_HINT,
                MSG_POSITION_HINT,
                MSG_PAUSED,
                MSG_MENU_PAUSE,
                MSG_PAUSE_CONTINUE,
                MSG_PAUSE_RESTART,
                MSG_PAUSE_MAIN_MENU,
                MSG_PAUSE_EXIT,
                MSG_CONFIRM_EXIT,
                MSG_EXIT_CANCELLED,
                MSG_RETURN_MAIN_MENU};
  int i;
  int j;
  int unique_ok;
  const char *zh;
  const char *en;

  unique_ok = 1;
  for (i = 0; i < (int)(sizeof(events) / sizeof(events[0])); i++) {
    for (j = i + 1; j < (int)(sizeof(events) / sizeof(events[0])); j++)
      if (events[i] == events[j])
        unique_ok = 0;
  }
  record_result("event center unique check", unique_ok);

  unique_ok = 1;
  for (i = 0; i < (int)(sizeof(keys) / sizeof(keys[0])); i++) {
    for (j = i + 1; j < (int)(sizeof(keys) / sizeof(keys[0])); j++)
      if (keys[i] == keys[j])
        unique_ok = 0;
  }
  record_result("message key center unique check", unique_ok);

  unique_ok = 1;
  for (i = 0; i < (int)(sizeof(keys) / sizeof(keys[0])); i++) {
    zh = msg_get(LANG_ZH_TW, keys[i]);
    en = msg_get(LANG_EN_US, keys[i]);
    if (zh == 0 || en == 0)
      unique_ok = 0;
    else if (strcmp(zh, "未知訊息") == 0 || strcmp(en, "Unknown message") == 0)
      unique_ok = 0;
  }
  record_result("message center mapping completeness", unique_ok);

  record_result("config center basic values",
                CFG_BOARD_SIZE_9 < CFG_BOARD_SIZE_15 &&
                    CFG_BOARD_SIZE_15 < CFG_BOARD_SIZE_19 &&
                    CFG_MAX_BOARD_SIZE == CFG_BOARD_SIZE_19 &&
                    CFG_INPUT_BUFFER_SIZE >= 16 && CFG_MESSAGE_HOLD_MS > 0);
}

int main(void) {
  printf("=== Phase F Validation Test ===\n");

  test_input_matrix();
  test_ai_matrix();
  test_smoke_win_draw_invalid();
  test_event_and_message_centers();

  printf("\n=== Phase F Summary ===\n");
  printf("Passed: %d/%d\n", g_pass, g_total);

  return (g_pass == g_total) ? 0 : 1;
}
