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
    {MSG_INTERNAL_ERROR, "系統錯誤", "Internal error"}};

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
