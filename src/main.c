#include "../include/config.h"
#include "../include/events.h"
#include "../include/functions.h"
#include "../include/message.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main() {
  int loop_res = EV_GAME_NONE, lang = LANG_ZH_TW;

  // 配置終端
  setup();

  while (1) {
    system(CONSOLE_CLEAR_CMD);
    system(msg_get(lang, MSG_WINDOW_TITLE));
    system(msg_get(lang, MSG_WINDOW_SIZE));

    loop_res = game_menu(lang);

    if (loop_res == EV_GAME_EXIT)
      break;
  }

  return 0;
}
