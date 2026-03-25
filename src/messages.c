#include "messages.h"
#include "config.h"

typedef struct MessagePair {
  int key;
  const char *zh_tw;
  const char *en_us;
} MessagePair;

static const MessagePair k_messages[] = {
    {MSG_WELCOME, "歡迎來到五子棋", "Welcome to Gomoku"},
    {MSG_READY, "系統就緒", "System ready"},
    {MSG_TURN_BLACK, "輪到黑子", "Black's turn"},
    {MSG_TURN_WHITE, "輪到白子", "White's turn"},
    {MSG_WIN_BLACK, "黑子獲勝", "Black wins"},
    {MSG_WIN_WHITE, "白子獲勝", "White wins"},
    {MSG_DRAW, "和局", "Draw"},
    {MSG_INVALID_INPUT, "輸入無效", "Invalid input"},
    {MSG_OUT_OF_BOUNDS, "落子超出邊界", "Move out of bounds"},
    {MSG_CELL_OCCUPIED, "該位置已有棋子", "Cell already occupied"},
    {MSG_NOT_YOUR_TURN, "目前不是此方回合", "Not this player's turn"},
    {MSG_GAME_OVER, "對局已結束", "Game is already over"},
    {MSG_INTERNAL_ERROR, "系統錯誤", "Internal error"},
    {MSG_MENU_MODE, "選擇遊戲模式", "Select game mode"},
    {MSG_MENU_BOARD, "選擇棋盤大小", "Select board size"},
    {MSG_MODE_1V1, "1v1 對戰", "1v1 Play"},
    {MSG_MODE_1V_AI_EASY, "1v AI (簡單)", "1v AI (Easy)"},
    {MSG_MODE_1V_AI_MEDIUM, "1v AI (中等)", "1v AI (Medium)"},
    {MSG_MODE_1V_AI_HARD, "1v AI (困難)", "1v AI (Hard)"},
    {MSG_BOARD_9X9, "9×9 棋盤", "9x9 Board"},
    {MSG_BOARD_15X15, "15×15 棋盤", "15x15 Board"},
    {MSG_BOARD_19X19, "19×19 棋盤", "19x19 Board"},
    {MSG_INPUT_HINT, "輸入位置 (如 a1) 或方向鍵移動",
     "Enter position (e.g. a1) or use arrow keys"},
    {MSG_MOVE_HINT, "ESC暫停 | 方向鍵移動 | Enter落子",
     "ESC pause | Arrows move | Enter place"},
    {MSG_POSITION_HINT, "位置: %c%d", "Position: %c%d"},
    {MSG_PAUSED, "遊戲已暫停", "Game paused"},
    {MSG_MENU_PAUSE, "暫停選單", "Pause menu"},
    {MSG_PAUSE_CONTINUE, "繼續遊戲", "Continue"},
    {MSG_PAUSE_RESTART, "重新開始", "Restart"},
    {MSG_PAUSE_MAIN_MENU, "回主選單", "Back to main menu"},
    {MSG_PAUSE_EXIT, "退出遊戲", "Exit game"},
    {MSG_CONFIRM_EXIT, "確定要退出？(1=是, 2=否)",
     "Confirm exit? (1=Yes, 2=No)"},
    {MSG_EXIT_CANCELLED, "已取消退出", "Exit cancelled"},
    {MSG_RETURN_MAIN_MENU, "返回主選單", "Return to main menu"},
    {MSG_INPUT_RANGE_HINT, "請輸入 %d-%d", "Please enter %d-%d"},
    {MSG_PRESS_ANY_KEY_BACK_MENU, "按任意鍵返回主選單...",
     "Press any key to return to main menu..."},
    {MSG_TRACE_EVENT_FMT, "[追蹤] EV=%d", "[TRACE] EV=%d"},
    {MSG_UI_TITLE_FMT, "=== 五子棋 %dx%d ===", "=== Gomoku %dx%d ==="},
    {MSG_UI_STATUS_BLACK_FMT, "目前: 黑子(X) | 步數: %d",
     "Current: Black(X) | Moves: %d"},
    {MSG_UI_STATUS_WHITE_FMT, "目前: 白子(O) | 步數: %d",
     "Current: White(O) | Moves: %d"},
    {MSG_UI_INPUT_LABEL, "[輸入] > ", "[INPUT] > "},
    {MSG_UI_SLOT_LABEL_FMT, "[提示區] %s", "[SLOT] %s"},
    {MSG_UI_MSG_LABEL_FMT, "[訊息] %s", "[MSG] %s"},
    {MSG_BOARD_TITLE_FMT, "棋盤 %dx%d", "Board %dx%d"}};

const char *msg_get(int lang, int key) {
  int i;
  int total;

  total = (int)(sizeof(k_messages) / sizeof(k_messages[0]));
  for (i = 0; i < total; i++) {
    if (k_messages[i].key == key) {
      if (lang == LANG_EN_US)
        return k_messages[i].en_us;
      return k_messages[i].zh_tw;
    }
  }

  if (lang == LANG_EN_US)
    return "Unknown message";
  return "未知訊息";
}
