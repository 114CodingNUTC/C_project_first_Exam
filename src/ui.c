#include "config.h"
#include "messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

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

void ui_set_message(UIState *state, int message_key, int is_error,
                    long long current_ms, long long hold_ms) {
  if (state == 0)
    return;
  state->message_key = message_key;
  state->message_is_error = is_error;
  state->message_display_end_ms = current_ms + hold_ms;
  state->board_dirty = 1;
}

int ui_message_expired(const UIState *state, long long current_ms) {
  if (state == 0)
    return 1;
  return current_ms >= state->message_display_end_ms;
}

static long long get_current_ms(void) { return GetTickCount64(); }

void ui_render_board(const GomokuGame *game) {
  int row;
  int col;

  if (game == 0)
    return;

  printf("Board %dx%d\n", game->board_size, game->board_size);
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      int stone;
      stone = game->board[row][col];
      if (stone == STONE_BLACK)
        printf("X ");
      else if (stone == STONE_WHITE)
        printf("O ");
      else
        printf(". ");
    }
    printf("\n");
  }
}

void ui_show_message(int message_key) {
  printf("%s\n", msg_get(LANG_DEFAULT, message_key));
}

static int is_win_line_position(const GomokuGame *game, int row, int col) {
  int i;
  if (game == 0 || game->win_line_count == 0)
    return 0;
  for (i = 0; i < game->win_line_count; i++)
    if (game->win_line[i][0] == row && game->win_line[i][1] == col)
      return 1;
  return 0;
}

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

  APPEND("\x1b[3J\x1b[2J\x1b[H");

  APPEND("=== Gomoku %dx%d ===\n", game->board_size, game->board_size);
  APPEND("Current: %s | Moves: %d\n\n",
         game->current_player == STONE_BLACK ? "Black(X)" : "White(O)",
         game->move_count);

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

      if (is_cursor)
        APPEND("[");
      else
        APPEND(" ");

      if (is_win) {
        if (stone == STONE_BLACK)
          APPEND("#");
        else if (stone == STONE_WHITE)
          APPEND("#");
        else
          APPEND("#");
      } else if (is_last) {
        if (stone == STONE_BLACK)
          APPEND("x");
        else if (stone == STONE_WHITE)
          APPEND("o");
        else
          APPEND(".");
      } else {
        if (stone == STONE_BLACK)
          APPEND("X");
        else if (stone == STONE_WHITE)
          APPEND("O");
        else
          APPEND(".");
      }

      if (is_cursor)
        APPEND("]");
      else
        APPEND(" ");
    }
    APPEND("%2d\n", row + 1);
  }

  APPEND("   ");
  for (col = 0; col < game->board_size; col++)
    APPEND(" %c ", 'A' + col);
  APPEND("\n");

  APPEND("\n");
  APPEND("[INPUT] > ");
  for (col = 0; col < state->input_length; col++) {
    if (col == state->input_cursor)
      APPEND("|");
    APPEND("%c", state->input_text[col]);
  }
  if (state->input_cursor == state->input_length)
    APPEND("|");
  APPEND("\n");
  APPEND("[SLOT] %s\n", msg_get(lang, MSG_INPUT_HINT));
  if (!ui_message_expired(state, current_ms))
    if (state->message_is_error)
      APPEND("[MSG] \x1b[31m%s\x1b[0m", msg_get(lang, state->message_key));
    else
      APPEND("[MSG] %s", msg_get(lang, state->message_key));
  else
    APPEND("[MSG] %s", msg_get(lang, MSG_MOVE_HINT));

  frame[offset] = '\0';
  fputs(frame, stdout);
  fflush(stdout);

#undef APPEND
}
