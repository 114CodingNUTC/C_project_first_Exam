#include "config.h"
#include "messages.h"
#include <stdio.h>
#include <windows.h>

static void setup_console_codepage(void) {
  SetConsoleOutputCP(CFG_CONSOLE_OUTPUT_CODEPAGE);
  SetConsoleCP(CFG_CONSOLE_INPUT_CODEPAGE);
}

int main(void) {
  int mode;
  int board_size;

  setup_console_codepage();

  while (1) {
    system(CFG_CLEAR_SCREEN_CMD);
    printf("%s\n\n", msg_get(LANG_DEFAULT, MSG_WELCOME));

    mode = input_choose_mode(LANG_DEFAULT);
    board_size = input_choose_board_size(LANG_DEFAULT);
    game_run_loop(mode, board_size, LANG_DEFAULT);
  }

  return 0;
}
