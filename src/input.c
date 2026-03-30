#include "../include/config.h"
#include "../include/events.h"
#include "../include/functions.h"
#include "../include/messages.h"
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/**
 * @brief 解析字母+數字格式座標（例如 a10）。
 * @param text 原始輸入字串。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表解析成功；0 代表格式不符。
 */
static int parse_letters_then_digits(const char *text, int *out_row,
                                     int *out_col) {
  int col;
  int row;

  if (!isalpha((unsigned char)text[0]))
    return 0;

  col = toupper((unsigned char)text[0]) - 'A';
  row = atoi(text + 1) - 1;
  if (row < 0 || col < 0)
    return 0;

  *out_row = row;
  *out_col = col;
  return 1;
}

/**
 * @brief 解析數字+字母格式座標（例如 10a）。
 * @param text 原始輸入字串。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表解析成功；0 代表格式不符。
 */
static int parse_digits_then_letters(const char *text, int *out_row,
                                     int *out_col) {
  int i;
  int col;
  int row;

  i = 0;
  while (isdigit((unsigned char)text[i]))
    i++;
  if (i == 0 || !isalpha((unsigned char)text[i]))
    return 0;

  row = atoi(text) - 1;
  col = toupper((unsigned char)text[i]) - 'A';
  if (row < 0 || col < 0)
    return 0;

  *out_row = row;
  *out_col = col;
  return 1;
}

/**
 * @brief 解析落子輸入並驗證是否在棋盤範圍內。
 * @param text 原始輸入字串。
 * @param board_size 棋盤邊長。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表成功；0 代表無效輸入。
 */
int input_parse_move(const char *text, int board_size, int *out_row,
                     int *out_col) {
  char normalized[32];
  int i;
  int j;

  if (text == 0 || out_row == 0 || out_col == 0 || board_size <= 0)
    return 0;

  j = 0;
  for (i = 0; text[i] != '\0' && j < 31; i++)
    if (text[i] != ' ' && text[i] != ',')
      normalized[j++] = text[i];
  normalized[j] = '\0';

  if (!parse_letters_then_digits(normalized, out_row, out_col) &&
      !parse_digits_then_letters(normalized, out_row, out_col)) {
    return 0;
  }

  if (*out_row >= board_size || *out_col >= board_size)
    return 0;

  return 1;
}

/**
 * @brief 判斷按鍵是否可作為文字輸入。
 * @param key 按鍵碼。
 * @return 1 代表可輸入；0 代表忽略。
 */
static int is_input_char(int key) {
  if (isalnum((unsigned char)key))
    return 1;
  if (key == ',' || key == ' ')
    return 1;
  return 0;
}

/**
 * @brief 在游標位置插入一個字元。
 * @param ui_state 目前 UI 狀態。
 * @param key 要插入的按鍵碼。
 */
static void input_append_char(UIState *ui_state, int key) {
  if (ui_state->input_length >= CFG_INPUT_BUFFER_SIZE - 1)
    return;
  memmove(&ui_state->input_text[ui_state->input_cursor + 1],
          &ui_state->input_text[ui_state->input_cursor],
          (size_t)(ui_state->input_length - ui_state->input_cursor));
  ui_state->input_text[ui_state->input_cursor] = (char)key;
  ui_state->input_cursor++;
  ui_state->input_length++;
  ui_state->input_text[ui_state->input_length] = '\0';
  ui_state->board_dirty = 1;
}

/**
 * @brief 刪除游標前一個字元（Backspace）。
 * @param ui_state 目前 UI 狀態。
 */
static void input_pop_char(UIState *ui_state) {
  if (ui_state->input_cursor <= 0 || ui_state->input_length <= 0)
    return;
  memmove(&ui_state->input_text[ui_state->input_cursor - 1],
          &ui_state->input_text[ui_state->input_cursor],
          (size_t)(ui_state->input_length - ui_state->input_cursor));
  ui_state->input_cursor--;
  ui_state->input_length--;
  ui_state->input_text[ui_state->input_length] = '\0';
  ui_state->board_dirty = 1;
}

/**
 * @brief 刪除游標所在字元（Delete）。
 * @param ui_state 目前 UI 狀態。
 */
static void input_delete_char(UIState *ui_state) {
  if (ui_state->input_cursor >= ui_state->input_length)
    return;
  memmove(&ui_state->input_text[ui_state->input_cursor],
          &ui_state->input_text[ui_state->input_cursor + 1],
          (size_t)(ui_state->input_length - ui_state->input_cursor - 1));
  ui_state->input_length--;
  ui_state->input_text[ui_state->input_length] = '\0';
  ui_state->board_dirty = 1;
}

/**
 * @brief 將輸入游標向左移動一格。
 * @param ui_state 目前 UI 狀態。
 */
static void input_move_left(UIState *ui_state) {
  if (ui_state->input_cursor > 0) {
    ui_state->input_cursor--;
    ui_state->board_dirty = 1;
  }
}

/**
 * @brief 將輸入游標向右移動一格。
 * @param ui_state 目前 UI 狀態。
 */
static void input_move_right(UIState *ui_state) {
  if (ui_state->input_cursor < ui_state->input_length) {
    ui_state->input_cursor++;
    ui_state->board_dirty = 1;
  }
}

/**
 * @brief 清空輸入框內容與游標位置。
 * @param ui_state 目前 UI 狀態。
 */
static void input_clear(UIState *ui_state) {
  ui_state->input_length = 0;
  ui_state->input_cursor = 0;
  ui_state->input_text[0] = '\0';
  ui_state->board_dirty = 1;
}

/**
 * @brief 讀取單一數字選項並限制在範圍內。
 * @param min_value 可接受最小值。
 * @param max_value 可接受最大值。
 * @param lang 語系。
 * @return 使用者輸入且通過驗證的值。
 */
static int read_number_in_range(int min_value, int max_value, int lang) {
  int key;
  int value;

  while (1) {
    key = _getch();

    if (key == CFG_KEY_CTRL_C)
      ExitProcess(0);

    if (!isdigit((unsigned char)key))
      continue;

    value = key - '0';
    if (value >= min_value && value <= max_value) {
      printf("%d\n", value);
      return value;
    }

    printf(msg_get(lang, MSG_INPUT_RANGE_HINT), min_value, max_value);
    printf("\n");
    printf("> ");
  }
}

/**
 * @brief 讀取遊戲模式選擇。
 * @param lang 語系。
 * @return 1-4 的模式代碼。
 */
int input_choose_mode(int lang) {
  printf("%s\n", msg_get(lang, MSG_MENU_MODE));
  printf("1) %s\n", msg_get(lang, MSG_MODE_1V1));
  printf("2) %s\n", msg_get(lang, MSG_MODE_1V_AI_EASY));
  printf("3) %s\n", msg_get(lang, MSG_MODE_1V_AI_MEDIUM));
  printf("4) %s\n", msg_get(lang, MSG_MODE_1V_AI_HARD));
  printf("> ");
  return read_number_in_range(1, 4, lang);
}

/**
 * @brief 讀取棋盤尺寸選擇。
 * @param lang 語系。
 * @return 對應的棋盤邊長常數。
 */
int input_choose_board_size(int lang) {
  printf("%s\n", msg_get(lang, MSG_MENU_BOARD));
  printf("1) %s\n", msg_get(lang, MSG_BOARD_9X9));
  printf("2) %s\n", msg_get(lang, MSG_BOARD_15X15));
  printf("3) %s\n", msg_get(lang, MSG_BOARD_19X19));
  printf("> ");

  switch (read_number_in_range(1, 3, lang)) {
  case 1:
    return CFG_BOARD_SIZE_9;
  case 2:
    return CFG_BOARD_SIZE_15;
  default:
    return CFG_BOARD_SIZE_19;
  }
}

/**
 * @brief 讀取 AI 先後手設定。
 * @param lang 語系。
 * @return CFG_AI_TURN_PLAYER_FIRST 或 CFG_AI_TURN_AI_FIRST。
 */
int input_choose_ai_turn(int lang) {
  printf("%s\n", msg_get(lang, MSG_MENU_AI_TURN));
  printf("1) %s\n", msg_get(lang, MSG_AI_TURN_PLAYER_FIRST));
  printf("2) %s\n", msg_get(lang, MSG_AI_TURN_AI_FIRST));
  printf("> ");
  return read_number_in_range(CFG_AI_TURN_PLAYER_FIRST, CFG_AI_TURN_AI_FIRST,
                              lang);
}

/**
 * @brief 讀取暫停選單操作。
 * @param lang 語系。
 * @return 1-4 的暫停操作代碼。
 */
int input_choose_pause_action(int lang) {
  printf("\n%s\n", msg_get(lang, MSG_MENU_PAUSE));
  printf("1) %s\n", msg_get(lang, MSG_PAUSE_CONTINUE));
  printf("2) %s\n", msg_get(lang, MSG_PAUSE_RESTART));
  printf("3) %s\n", msg_get(lang, MSG_PAUSE_MAIN_MENU));
  printf("4) %s\n", msg_get(lang, MSG_PAUSE_EXIT));
  printf("> ");
  return read_number_in_range(1, 4, lang);
}

/**
 * @brief 讀取退出確認。
 * @param lang 語系。
 * @return 1 代表確認退出；0 代表取消。
 */
int input_confirm_exit(int lang) {
  printf("%s\n", msg_get(lang, MSG_CONFIRM_EXIT));
  printf("> ");
  return read_number_in_range(1, 2, lang) == 1;
}

/**
 * @brief 讀取玩家落子行為（文字輸入或方向鍵游標）。
 * @param game 當前遊戲狀態。
 * @param ui_state 目前 UI 狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return PLAYER_ACTION_* 動作碼（落子/暫停/投降）。
 */
int input_read_player_move(const GomokuGame *game, UIState *ui_state,
                           int *out_row, int *out_col) {
  int key;
  int ext;
  long long now_ms;

  while (1) {
    ui_render_full(game, ui_state, LANG_DEFAULT);
    key = _getch();

    if (key == CFG_KEY_CTRL_C)
      ExitProcess(0);

    if (key == CFG_KEY_ESC)
      return PLAYER_ACTION_PAUSE;

    if (key == CFG_KEY_TAB)
      return PLAYER_ACTION_SURRENDER;

    if (key == CFG_KEY_ENTER) {
      if (ui_state->input_length > 0) {
        if (input_parse_move(ui_state->input_text, game->board_size, out_row,
                             out_col)) {
          ui_move_cursor(ui_state, *out_row, *out_col, game->board_size);
          input_clear(ui_state);
          return PLAYER_ACTION_PLACE;
        }
        now_ms = GetTickCount64();
        ui_set_message(ui_state, MSG_INVALID_INPUT, 1, now_ms,
                       CFG_MESSAGE_HOLD_MS);
        continue;
      }
      *out_row = ui_state->cursor_row;
      *out_col = ui_state->cursor_col;
      return PLAYER_ACTION_PLACE;
    }

    if (key == CFG_KEY_BACKSPACE) {
      input_pop_char(ui_state);
      continue;
    }

    if (key == CFG_KEY_EXTENDED_0 || key == CFG_KEY_EXTENDED_224) {
      ext = _getch();
      if (ext == CFG_KEY_DELETE) {
        input_delete_char(ui_state);
        continue;
      }
      if (ext == CFG_KEY_UP) {
        ui_move_cursor(ui_state, ui_state->cursor_row - 1, ui_state->cursor_col,
                       game->board_size);
      } else if (ext == CFG_KEY_DOWN) {
        ui_move_cursor(ui_state, ui_state->cursor_row + 1, ui_state->cursor_col,
                       game->board_size);
      } else if (ext == CFG_KEY_LEFT) {
        if (ui_state->input_length > 0) {
          input_move_left(ui_state);
        } else {
          ui_move_cursor(ui_state, ui_state->cursor_row,
                         ui_state->cursor_col - 1, game->board_size);
        }
      } else if (ext == CFG_KEY_RIGHT) {
        if (ui_state->input_length > 0) {
          input_move_right(ui_state);
        } else {
          ui_move_cursor(ui_state, ui_state->cursor_row,
                         ui_state->cursor_col + 1, game->board_size);
        }
      }
      continue;
    }

    if (is_input_char(key))
      input_append_char(ui_state, key);
  }
}

/**
 * @brief 使用視覺化菜單選擇遊戲模式（含光標上下移動）。
 * @param lang 語系。
 * @return 1-4 的模式代碼。
 */
int input_choose_mode_visual(int lang) {
  int current_selection = 0;
  int key;
  int ext;
  const char *mode_options[] = {
      msg_get(lang, MSG_MODE_1V1), msg_get(lang, MSG_MODE_1V_AI_EASY),
      msg_get(lang, MSG_MODE_1V_AI_MEDIUM), msg_get(lang, MSG_MODE_1V_AI_HARD)};

  system(CFG_CLEAR_SCREEN_CMD);
  ui_hide_cursor();

  /* 初始繪製菜單（只繪製一次） */
  ui_draw_menu_title();
  ui_draw_menu_options(current_selection, 4, mode_options);
  ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));

  while (1) {
    if (_kbhit()) {
      key = _getch();

      if (key == CFG_KEY_CTRL_C)
        ExitProcess(0);

      if (key == CFG_KEY_EXTENDED_0 || key == CFG_KEY_EXTENDED_224) {
        ext = _getch();
        if (ext == CFG_KEY_UP)
          current_selection = current_selection > 0 ? current_selection - 1 : 3;
        if (ext == CFG_KEY_DOWN)
          current_selection = current_selection < 3 ? current_selection + 1 : 0;

        /* 只在獲得輸入時重繪 */
        system(CFG_CLEAR_SCREEN_CMD);
        ui_draw_menu_title();
        ui_draw_menu_options(current_selection, 4, mode_options);
        ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));
      } else if (key == 'w' || key == 'W') {
        current_selection = 0;
        system(CFG_CLEAR_SCREEN_CMD);
        ui_draw_menu_title();
        ui_draw_menu_options(current_selection, 4, mode_options);
        ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));
      } else if (key == 's' || key == 'S') {
        current_selection = 0;
        system(CFG_CLEAR_SCREEN_CMD);
        ui_draw_menu_title();
        ui_draw_menu_options(current_selection, 4, mode_options);
        ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));
      } else if (key == CFG_KEY_ENTER) {
        return current_selection + 1;
      }
    }

    Sleep(50);
  }
}

/**
 * @brief 使用視覺化菜單選擇棋盤大小（含光標上下移動）。
 * @param lang 語系。
 * @return 對應的棋盤邊長常數。
 */
int input_choose_board_size_visual(int lang) {
  int current_selection = 1;
  int key;
  int ext;
  const char *size_options[] = {msg_get(lang, MSG_BOARD_9X9),
                                msg_get(lang, MSG_BOARD_15X15),
                                msg_get(lang, MSG_BOARD_19X19)};

  system(CFG_CLEAR_SCREEN_CMD);
  ui_hide_cursor();

  /* 初始繪製菜單（只繪製一次） */
  ui_draw_menu_title();
  ui_draw_menu_options(current_selection, 3, size_options);
  ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));

  while (1) {
    if (_kbhit()) {
      key = _getch();

      if (key == CFG_KEY_CTRL_C)
        ExitProcess(0);

      if (key == CFG_KEY_EXTENDED_0 || key == CFG_KEY_EXTENDED_224) {
        ext = _getch();
        if (ext == CFG_KEY_UP)
          current_selection = current_selection > 0 ? current_selection - 1 : 2;
        if (ext == CFG_KEY_DOWN)
          current_selection = current_selection < 2 ? current_selection + 1 : 0;

        /* 只在獲得輸入時重繪 */
        system(CFG_CLEAR_SCREEN_CMD);
        ui_draw_menu_title();
        ui_draw_menu_options(current_selection, 3, size_options);
        ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));
      } else if (key == CFG_KEY_ENTER) {
        switch (current_selection) {
        case 0:
          return CFG_BOARD_SIZE_9;
        case 1:
          return CFG_BOARD_SIZE_15;
        default:
          return CFG_BOARD_SIZE_19;
        }
      }
    }

    Sleep(50);
  }
}

/**
 * @brief 使用視覺化菜單選擇 AI 先後手（含光標上下移動）。
 * @param lang 語系。
 * @return CFG_AI_TURN_PLAYER_FIRST 或 CFG_AI_TURN_AI_FIRST。
 */
int input_choose_ai_turn_visual(int lang) {
  int current_selection = 0;
  int key;
  int ext;
  const char *turn_options[] = {msg_get(lang, MSG_AI_TURN_PLAYER_FIRST),
                                msg_get(lang, MSG_AI_TURN_AI_FIRST)};

  system(CFG_CLEAR_SCREEN_CMD);
  ui_hide_cursor();

  /* 初始繪製菜單（只繪製一次） */
  ui_draw_menu_title();
  ui_draw_menu_options(current_selection, 2, turn_options);
  ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));

  while (1) {
    if (_kbhit()) {
      key = _getch();

      if (key == CFG_KEY_CTRL_C)
        ExitProcess(0);

      if (key == CFG_KEY_EXTENDED_0 || key == CFG_KEY_EXTENDED_224) {
        ext = _getch();
        if (ext == CFG_KEY_UP)
          current_selection = current_selection > 0 ? current_selection - 1 : 1;
        if (ext == CFG_KEY_DOWN)
          current_selection = current_selection < 1 ? current_selection + 1 : 0;

        /* 只在獲得輸入時重繪 */
        system(CFG_CLEAR_SCREEN_CMD);
        ui_draw_menu_title();
        ui_draw_menu_options(current_selection, 2, turn_options);
        ui_draw_menu_hint(msg_get(lang, MSG_MENU_HINT));
      } else if (key == CFG_KEY_ENTER) {
        return current_selection == 0 ? CFG_AI_TURN_PLAYER_FIRST
                                      : CFG_AI_TURN_AI_FIRST;
      }
    }

    Sleep(50);
  }
}
