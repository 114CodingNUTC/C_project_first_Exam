#include "../include/config.h"
#include "../include/functions.h"
#include "../include/messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/**
 * @brief 初始化 UI 狀態。
 * @param state UI 狀態物件。
 * @param board_size 棋盤邊長。
 */
void ui_init_state(UIState *state, int board_size) {
  if (state == 0)
    return;
  state->cursor_row = board_size / 2;
  state->cursor_col = board_size / 2;
  state->message_key = MSG_MOVE_HINT;
  state->message_display_end_ms = 0;
  state->message_is_error = 0;
  state->input_text[0] = '\0';
  state->input_length = 0;
  state->input_cursor = 0;
  state->ai_mode = 0;
  state->ai_difficulty = 0;
}

/**
 * @brief 移動游標並限制於棋盤範圍。
 * @param state UI 狀態物件。
 * @param row 目標列索引。
 * @param col 目標行索引。
 * @param board_size 棋盤邊長。
 */
void ui_move_cursor(UIState *state, int row, int col, int board_size) {
  if (state == 0 || board_size <= 0)
    return;
  if (row < 0)
    row = 0;
  if (row >= board_size)
    row = board_size - 1;
  if (col < 0)
    col = 0;
  if (col >= board_size)
    col = board_size - 1;
  state->cursor_row = row;
  state->cursor_col = col;
}

/**
 * @brief 設定提示訊息與顯示時效。
 * @param state UI 狀態物件。
 * @param message_key 訊息鍵值。
 * @param is_error 是否為錯誤訊息。
 * @param current_ms 當前時間毫秒。
 * @param hold_ms 顯示持續毫秒。
 */
void ui_set_message(UIState *state, int message_key, int is_error,
                    long long current_ms, long long hold_ms) {
  if (state == 0)
    return;
  state->message_key = message_key;
  state->message_is_error = is_error;
  state->message_display_end_ms = current_ms + hold_ms;
}

/**
 * @brief 檢查訊息是否過期。
 * @param state UI 狀態物件。
 * @param current_ms 當前時間毫秒。
 * @return 1 代表訊息過期；0 代表仍有效。
 */
int ui_message_expired(const UIState *state, long long current_ms) {
  if (state == 0)
    return 1;
  return current_ms >= state->message_display_end_ms;
}

/**
 * @brief 取得目前毫秒時間戳。
 * @return 從系統啟動至今的毫秒數。
 */
static long long get_current_ms(void) { return (long long)GetTickCount64(); }

/**
 * @brief 清空主控台畫面並將游標回到原點。
 */
static void ui_clear_console(void) {
  HANDLE console;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  COORD home;
  DWORD length;
  DWORD written;

  console = GetStdHandle(STD_OUTPUT_HANDLE);
  if (console == INVALID_HANDLE_VALUE || console == 0)
    return;

  if (!GetConsoleScreenBufferInfo(console, &csbi))
    return;

  length = (DWORD)(csbi.dwSize.X * csbi.dwSize.Y);
  home.X = 0;
  home.Y = 0;

  FillConsoleOutputCharacter(console, ' ', length, home, &written);
  FillConsoleOutputAttribute(console, csbi.wAttributes, length, home, &written);
  SetConsoleCursorPosition(console, home);
}

/**
 * @brief 判斷指定座標是否屬於終局成線位置。
 * @param game 遊戲狀態物件。
 * @param row 目標列索引。
 * @param col 目標行索引。
 * @return 1 代表屬於成線；0 代表否。
 */
static int is_win_line_position(const GomokuGame *game, int row, int col) {
  int i;
  if (game == 0 || game->win_line_count == 0)
    return 0;
  for (i = 0; i < game->win_line_count; i++)
    if (game->win_line[i][0] == row && game->win_line[i][1] == col)
      return 1;
  return 0;
}

/**
 * @brief 重新渲染完整遊戲畫面（棋盤、輸入列、訊息槽）。
 * @param game 遊戲狀態物件。
 * @param state UI 狀態物件。
 * @param lang 語系。
 */
void ui_render_full(const GomokuGame *game, const UIState *state, int lang) {
  int row;
  int col;
  long long current_ms;
  int offset;
  char frame[32768];

#define APPEND(...)                                                            \
  do {                                                                         \
    int _n;                                                                    \
    _n =                                                                       \
        snprintf(frame + offset, sizeof(frame) - (size_t)offset, __VA_ARGS__); \
    if (_n > 0) {                                                              \
      if ((size_t)_n >= sizeof(frame) - (size_t)offset)                        \
        offset = (int)sizeof(frame) - 1;                                       \
      else                                                                     \
        offset += _n;                                                          \
    }                                                                          \
  } while (0)

  if (game == 0 || state == 0)
    return;

  current_ms = get_current_ms();
  offset = 0;

  ui_clear_console();

  APPEND(msg_get(lang, MSG_UI_TITLE_FMT), game->board_size, game->board_size);
  APPEND("\n");
  if (game->current_player == STONE_BLACK)
    APPEND(msg_get(lang, MSG_UI_STATUS_BLACK_FMT), game->move_count);
  else
    APPEND(msg_get(lang, MSG_UI_STATUS_WHITE_FMT), game->move_count);
  if (state->ai_mode != 1) {
    APPEND(" | ");
    if (state->ai_difficulty == 2)
      APPEND(msg_get(lang, MSG_MODE_1V_AI_EASY));
    else if (state->ai_difficulty == 3)
      APPEND(msg_get(lang, MSG_MODE_1V_AI_MEDIUM));
    else if (state->ai_difficulty == 4)
      APPEND(msg_get(lang, MSG_MODE_1V_AI_HARD));
  }
  APPEND("\n\n");

  APPEND("  ");
  for (col = 0; col < game->board_size; col++)
    APPEND(" %c ", 'A' + col);
  APPEND("\n");

  for (row = 0; row < game->board_size; row++) {
    APPEND("%2d", row + 1);
    for (col = 0; col < game->board_size; col++) {
      int stone;
      int is_cursor = (state->cursor_row == row && state->cursor_col == col);
      int is_last = (game->last_row == row && game->last_col == col);
      int is_win = is_win_line_position(game, row, col);

      stone = game->board[row][col];

      if (is_win) {
        if (stone == STONE_BLACK) {
          APPEND("[%s%c%s]", CFG_ANSI_COLOR_BRIGHT_BLUE, CFG_STONE_CHAR_BLACK,
                 CFG_ANSI_COLOR_RESET);
        } else if (stone == STONE_WHITE) {
          APPEND("[%s%c%s]", CFG_ANSI_COLOR_BRIGHT_YELLOW, CFG_STONE_CHAR_WHITE,
                 CFG_ANSI_COLOR_RESET);
        } else {
          APPEND("[%c]", CFG_STONE_CHAR_WIN_FALLBACK);
        }
      } else {
        if (is_cursor)
          APPEND("[");
        else
          APPEND(" ");

        if (is_last) {
          if (stone == STONE_BLACK) {
            APPEND("%s%c%s", CFG_ANSI_COLOR_BRIGHT_BLUE,
                   CFG_STONE_CHAR_BLACK_LAST, CFG_ANSI_COLOR_RESET);
          } else if (stone == STONE_WHITE) {
            APPEND("%s%c%s", CFG_ANSI_COLOR_BRIGHT_YELLOW,
                   CFG_STONE_CHAR_WHITE_LAST, CFG_ANSI_COLOR_RESET);
          } else {
            APPEND("%c", CFG_STONE_CHAR_EMPTY);
          }
        } else if (stone == STONE_BLACK) {
          APPEND("%s%c%s", CFG_ANSI_COLOR_BRIGHT_BLUE, CFG_STONE_CHAR_BLACK,
                 CFG_ANSI_COLOR_RESET);
        } else if (stone == STONE_WHITE) {
          APPEND("%s%c%s", CFG_ANSI_COLOR_BRIGHT_YELLOW, CFG_STONE_CHAR_WHITE,
                 CFG_ANSI_COLOR_RESET);
        } else {
          APPEND("%c", CFG_STONE_CHAR_EMPTY);
        }

        if (is_cursor)
          APPEND("]");
        else
          APPEND(" ");
      }
    }
    APPEND("%2d\n", row + 1);
  }

  APPEND("  ");
  for (col = 0; col < game->board_size; col++)
    APPEND(" %c ", 'A' + col);
  APPEND("\n");

  APPEND("\n");
  APPEND("%s", msg_get(lang, MSG_UI_INPUT_LABEL));
  for (col = 0; col < state->input_length; col++) {
    if (col == state->input_cursor)
      APPEND("|");
    APPEND("%c", state->input_text[col]);
  }
  if (state->input_cursor == state->input_length)
    APPEND("|");
  APPEND("\n");
  APPEND(msg_get(lang, MSG_UI_SLOT_LABEL_FMT), msg_get(lang, MSG_INPUT_HINT));
  APPEND("\n");
  if (!ui_message_expired(state, current_ms)) {
    if (state->message_is_error) {
      APPEND("%s", CFG_ANSI_COLOR_RED);
      APPEND(msg_get(lang, MSG_UI_MSG_LABEL_FMT),
             msg_get(lang, state->message_key));
      APPEND("%s", CFG_ANSI_COLOR_RESET);
    } else {
      APPEND(msg_get(lang, MSG_UI_MSG_LABEL_FMT),
             msg_get(lang, state->message_key));
    }
  } else {
    APPEND(msg_get(lang, MSG_UI_MSG_LABEL_FMT), msg_get(lang, MSG_MOVE_HINT));
  }
  APPEND("\n");

  frame[offset] = '\0';
  fputs(frame, stdout);
  fflush(stdout);

#undef APPEND
}

/**
 * @brief 設定主控台文字顏色。
 * @param color 顏色代碼 (Windows 控制台顏色)。
 */
void ui_set_color(int color) {
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  if (console != INVALID_HANDLE_VALUE && console != 0)
    SetConsoleTextAttribute(console, (WORD)color);
}

/**
 * @brief 定位游標到指定位置 (X: 列, Y: 列)。
 * @param x 欄位置。
 * @param y 列位置。
 */
void ui_gotoxy(int x, int y) {
  COORD coord;
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  coord.X = (SHORT)x;
  coord.Y = (SHORT)y;
  if (console != INVALID_HANDLE_VALUE && console != 0)
    SetConsoleCursorPosition(console, coord);
}

/**
 * @brief 隱藏主控台游標。
 */
void ui_hide_cursor(void) {
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO info;
  if (console == INVALID_HANDLE_VALUE || console == 0)
    return;
  info.dwSize = 100;
  info.bVisible = FALSE;
  SetConsoleCursorInfo(console, &info);
}

static int ui_utf8_display_width(const char *text);
static int ui_center_x_for_text(const char *text, int extra_width);

/**
 * @brief 繪製選單標題 (ASCII Art GOMOKU)。
 */
void ui_draw_menu_title(int lang) {
  const char *subtitle;

  ui_set_color(CFG_COLOR_CYAN);
  ui_gotoxy(0, CFG_MENU_TITLE_ROW);
  printf(
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
      "\\______/   ]");

  subtitle = msg_get(lang, MSG_MENU_SUBTITLE);
  ui_set_color(CFG_COLOR_YELLOW);
  ui_gotoxy(ui_center_x_for_text(subtitle, 0), CFG_MENU_SUBTITLE_ROW);
  printf("%s", subtitle);

  ui_set_color(CFG_COLOR_WHITE);
}

/**
 * @brief 計算 UTF-8 字串在主控台的大致顯示寬度。
 * @param text UTF-8 字串。
 * @return 估算的欄位寬度。
 */
static int ui_utf8_display_width(const char *text) {
  int width;
  const unsigned char *p;

  if (text == 0)
    return 0;

  width = 0;
  p = (const unsigned char *)text;

  while (*p != '\0') {
    if (*p < 0x80) {
      width += 1;
      p += 1;
    } else if ((*p & 0xF0) == 0xE0 && p[1] != '\0' && p[2] != '\0') {
      width += 2;
      p += 3;
    } else if ((*p & 0xE0) == 0xC0 && p[1] != '\0') {
      width += 1;
      p += 2;
    } else if ((*p & 0xF8) == 0xF0 && p[1] != '\0' && p[2] != '\0' &&
               p[3] != '\0') {
      width += 2;
      p += 4;
    } else {
      width += 1;
      p += 1;
    }
  }

  return width;
}

/**
 * @brief 依文字顯示寬度計算置中 X 座標。
 * @param text 目標文字。
 * @param extra_width 文字前後固定符號的額外寬度。
 * @return 置中後的 X。
 */
static int ui_center_x_for_text(const char *text, int extra_width) {
  int total_width;
  int x;

  total_width = ui_utf8_display_width(text) + extra_width;
  x = (CFG_MENU_CONSOLE_WIDTH - total_width) / 2;
  if (x < 0)
    return 0;
  return x;
}

/**
 * @brief 繪製選單選項。
 * @param current_selection 當前選中選項 (0-based)。
 * @param option_count 選項總數。
 * @param option_texts 選項文本陣列。
 */
void ui_draw_menu_options(int current_selection, int option_count,
                          const char **option_texts) {
  int i;
  int row = CFG_MENU_OPTION1_ROW;
  int x;

  if (option_texts == 0 || option_count <= 0)
    return;

  for (i = 0; i < option_count; i++) {
    x = ui_center_x_for_text(option_texts[i], 6);
    ui_gotoxy(x, row + (i * 2));

    if (i == current_selection) {
      ui_set_color(CFG_COLOR_RED);
      printf(">> %s <<", option_texts[i]);
    } else {
      ui_set_color(CFG_COLOR_WHITE);
      printf("   %s   ", option_texts[i]);
    }
  }

  ui_set_color(CFG_COLOR_WHITE);
}

/**
 * @brief 繪製菜單提示文本。
 * @param hint_text 提示文本。
 */
void ui_draw_menu_hint(const char *hint_text) {
  int x;

  ui_set_color(CFG_COLOR_WHITE);
  x = ui_center_x_for_text(hint_text, 0);
  ui_gotoxy(x, CFG_MENU_HINT_ROW);
  if (hint_text != 0)
    printf("%s", hint_text);
}
