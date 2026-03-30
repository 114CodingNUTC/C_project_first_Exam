#include "../include/config.h"
#include "../include/events.h"
#include "../include/functions.h"
#include "../include/messages.h"
#include <stdio.h>
#include <windows.h>

/**
 * @brief 攔截 Ctrl+C 與 Ctrl+Break，直接結束程式。
 * @param ctrl_type 主控台控制訊號代碼。
 * @return TRUE 代表已處理；FALSE 代表交由系統後續處理。
 */
static BOOL WINAPI handle_console_ctrl(DWORD ctrl_type) {
  if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_BREAK_EVENT) {
    ExitProcess(0);
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief 設定主控台輸入/輸出編碼與控制訊號處理器。
 */
static void setup_console_codepage(void) {
  SetConsoleOutputCP(CFG_CONSOLE_OUTPUT_CODEPAGE);
  SetConsoleCP(CFG_CONSOLE_INPUT_CODEPAGE);
  SetConsoleCtrlHandler(handle_console_ctrl, TRUE);
}

/**
 * @brief 程式主入口，負責主選單迴圈與對局啟動。
 * @return 0 代表正常結束。
 */
int main(void) {
  int mode;
  int board_size;
  int ai_turn_choice;
  int loop_result;

  setup_console_codepage();

  while (1) {
    system(CFG_CLEAR_SCREEN_CMD);

    mode = input_choose_mode_visual(LANG_DEFAULT);
    board_size = input_choose_board_size_visual(LANG_DEFAULT);
    ai_turn_choice = CFG_AI_TURN_PLAYER_FIRST;
    if (mode != 1)
      ai_turn_choice = input_choose_ai_turn_visual(LANG_DEFAULT);

    loop_result = game_run_loop(mode, board_size, ai_turn_choice, LANG_DEFAULT);
    if (loop_result == GAME_LOOP_EXIT_APP)
      break;
  }

  return 0;
}
