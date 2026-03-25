#include "config.h"
#include "events.h"
#include "messages.h"
#include <stdio.h>
#include <windows.h>

static BOOL WINAPI handle_console_ctrl(DWORD ctrl_type) {
  if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_BREAK_EVENT) {
    ExitProcess(0);
    return TRUE;
  }
  return FALSE;
}

static void setup_console_codepage(void) {
  SetConsoleOutputCP(CFG_CONSOLE_OUTPUT_CODEPAGE);
  SetConsoleCP(CFG_CONSOLE_INPUT_CODEPAGE);
  SetConsoleCtrlHandler(handle_console_ctrl, TRUE);
}

int main(void) {
  int mode;
  int board_size;
  int ai_turn_choice;
  int loop_result;

  setup_console_codepage();

  while (1) {
    system(CFG_CLEAR_SCREEN_CMD);
    printf("%s\n\n", msg_get(LANG_DEFAULT, MSG_WELCOME));

    mode = input_choose_mode(LANG_DEFAULT);
    board_size = input_choose_board_size(LANG_DEFAULT);
    ai_turn_choice = CFG_AI_TURN_PLAYER_FIRST;
    if (mode != 1)
      ai_turn_choice = input_choose_ai_turn(LANG_DEFAULT);

    loop_result = game_run_loop(mode, board_size, ai_turn_choice, LANG_DEFAULT);
    if (loop_result == GAME_LOOP_EXIT_APP)
      break;
  }

  return 0;
}
