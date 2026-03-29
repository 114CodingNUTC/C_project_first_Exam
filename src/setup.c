#include "../include/config.h"
#include <windows.h>

/**
 * 配置終端 (UTF-8 編碼、控制事件處理)
 * @return void
 */
static boolean WINAPI force_Exit(DWORD ctrl_type) {
  if (ctrl_type == CTRL_C_EVENT || ctrl_type == CTRL_CLOSE_EVENT ||
      ctrl_type == CTRL_BREAK_EVENT || ctrl_type == CTRL_LOGOFF_EVENT ||
      ctrl_type == CTRL_SHUTDOWN_EVENT) {
    exitGame();
    ExitProcess(0);
    return TRUE;
  }
  return FALSE;
}

static void setup() {
  // 配置終端
  SetConsoleCP(CONSOLE_UTF8);
  SetConsoleOutputCP(CONSOLE_UTF8);
  SetConsoleCtrlHandler(force_Exit, TRUE);
}

/**
 * 退出遊戲
 */
static void exitGame() {
  SetConsoleCP(0);
  SetConsoleOutputCP(0);
}
