#include "../include/message.h"
#include "../include/config.h"

typedef struct GameMessage {
  int key;
  const char *zh_tw;
} GameMessage;

static const GameMessage game_messages[] = {
    {MSG_LOGO,
     "     [  ______   ______   .___  ___.   ______    __  ___  __    __   ]\n"
     "     [ / ___  | /  __  \\  |   \\/   |  /  __  \\  |  |/  / |  |  |  |  "
     "]\n"
     "     [| |   \\_||  |  |  | |  \\  /  | |  |  |  | |  '  /  |  |  |  |  "
     "]\n"
     "     [| |  ___ |  |  |  | |  |\\/|  | |  |  |  | |    |   |  |  |  |  "
     "]\n"
     "     [| |__/  ||  `--'  | |  |  |  | |  `--'  | |  .  \\  |  `--'  |  "
     "]\n"
     "     [ \\______| \\______/  |__|  |__|  \\______/  |__|\\__\\  "
     "\\______/   ]"},
    {MSG_WINDOW_TITLE, "title GOMOKU - 五子棋"},
    {MSG_WINDOW_SIZE, "mode con cols=75 lines=25"},
    {MSG_SUBTITLE, "=== 五 子 棋 ==="},
};

/**
 * 根據語言和訊息鍵獲取對應的訊息文本
 * @param lang 語言代碼 (目前僅支持 LANG_ZH_TW)
 * @param key 訊息鍵 (如 MSG_LOGO)
 * @return 對應的訊息文本，如果未找到則返回 "未知訊息"
 */
const char *msg_get(int lang, int key) {
  int i, total;
  total = sizeof(game_messages) / sizeof(GameMessage);
  for (i = 0; i < total; i++)
    if (game_messages[i].key == key)
      return game_messages[i].zh_tw;
  return "未知訊息";
}
