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
  state->board_dirty = 1;
  state->input_text[0] = '\0';
  state->input_length = 0;
  state->input_cursor = 0;
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
  state->board_dirty = 1;
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
  state->board_dirty = 1;
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
static long long get_current_ms(void) { return GetTickCount64(); }

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
 * @brief 繪製純棋盤內容（簡易版本）。
 * @param game 遊戲狀態物件。
 */
void ui_render_board(const GomokuGame *game) {
  int row;
  int col;

  if (game == 0)
    return;

  printf(msg_get(LANG_DEFAULT, MSG_BOARD_TITLE_FMT), game->board_size,
         game->board_size);
  printf("\n");
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      int stone;
      stone = game->board[row][col];
      if (stone == STONE_BLACK)
        printf("%c ", CFG_STONE_CHAR_BLACK);
      else if (stone == STONE_WHITE)
        printf("%c ", CFG_STONE_CHAR_WHITE);
      else
        printf("%c ", CFG_STONE_CHAR_EMPTY);
    }
    printf("\n");
  }
}

/**
 * @brief 顯示單行訊息。
 * @param message_key 訊息鍵值。
 */
void ui_show_message(int message_key) {
  printf("%s\n", msg_get(LANG_DEFAULT, message_key));
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
  APPEND("\n\n");

  APPEND("   ");
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
        if (stone == STONE_BLACK)
          APPEND("[%c]", CFG_STONE_CHAR_BLACK);
        else if (stone == STONE_WHITE)
          APPEND("[%c]", CFG_STONE_CHAR_WHITE);
        else
          APPEND("[%c]", CFG_STONE_CHAR_WIN_FALLBACK);
      } else {
        if (is_cursor)
          APPEND("[");
        else
          APPEND(" ");

        if (is_last) {
        if (stone == STONE_BLACK)
          APPEND("%c", CFG_STONE_CHAR_BLACK_LAST);
        else if (stone == STONE_WHITE)
          APPEND("%c", CFG_STONE_CHAR_WHITE_LAST);
        else
          APPEND("%c", CFG_STONE_CHAR_EMPTY);
        } else {
          if (stone == STONE_BLACK)
            APPEND("%c", CFG_STONE_CHAR_BLACK);
          else if (stone == STONE_WHITE)
            APPEND("%c", CFG_STONE_CHAR_WHITE);
          else
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

  APPEND("   ");
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

  frame[offset] = '\0';
  fputs(frame, stdout);
  fflush(stdout);

#undef APPEND
}
