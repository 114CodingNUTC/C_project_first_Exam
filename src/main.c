#include "config.h"
#include "events.h"
#include "messages.h"
#include <stdio.h>
#include <windows.h>

static void setup_console_codepage(void) {
  SetConsoleOutputCP(CFG_CONSOLE_OUTPUT_CODEPAGE);
  SetConsoleCP(CFG_CONSOLE_INPUT_CODEPAGE);
}

int main(void) {
  GomokuGame game;
  int event_code;
  int msg_key;
  int result;

  setup_console_codepage();

  printf("%s\n", msg_get(LANG_DEFAULT, MSG_WELCOME));

  game_reset(&game, CFG_BOARD_SIZE_DEFAULT);
  event_code = EV_READY;
  msg_key = MSG_READY;
  printf("[EV:%d] %s\n", event_code, msg_get(LANG_DEFAULT, msg_key));

  result =
      try_place_stone(&game, game.current_player, 7, 7, &event_code, &msg_key);
  printf("result=%d [EV:%d] %s\n", result, event_code,
         msg_get(LANG_DEFAULT, msg_key));

  return 0;
}
