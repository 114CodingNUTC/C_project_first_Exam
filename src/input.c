#include "config.h"
#include "messages.h"
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

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

static int is_input_char(int key) {
  if (isalnum((unsigned char)key))
    return 1;
  if (key == ',' || key == ' ')
    return 1;
  return 0;
}

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

static void input_move_left(UIState *ui_state) {
  if (ui_state->input_cursor > 0) {
    ui_state->input_cursor--;
    ui_state->board_dirty = 1;
  }
}

static void input_move_right(UIState *ui_state) {
  if (ui_state->input_cursor < ui_state->input_length) {
    ui_state->input_cursor++;
    ui_state->board_dirty = 1;
  }
}

static void input_clear(UIState *ui_state) {
  ui_state->input_length = 0;
  ui_state->input_cursor = 0;
  ui_state->input_text[0] = '\0';
  ui_state->board_dirty = 1;
}

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

int input_choose_mode(int lang) {
  printf("%s\n", msg_get(lang, MSG_MENU_MODE));
  printf("1) %s\n", msg_get(lang, MSG_MODE_1V1));
  printf("2) %s\n", msg_get(lang, MSG_MODE_1V_AI_EASY));
  printf("3) %s\n", msg_get(lang, MSG_MODE_1V_AI_MEDIUM));
  printf("4) %s\n", msg_get(lang, MSG_MODE_1V_AI_HARD));
  printf("> ");
  return read_number_in_range(1, 4, lang);
}

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

int input_choose_pause_action(int lang) {
  printf("\n%s\n", msg_get(lang, MSG_MENU_PAUSE));
  printf("1) %s\n", msg_get(lang, MSG_PAUSE_CONTINUE));
  printf("2) %s\n", msg_get(lang, MSG_PAUSE_RESTART));
  printf("3) %s\n", msg_get(lang, MSG_PAUSE_MAIN_MENU));
  printf("4) %s\n", msg_get(lang, MSG_PAUSE_EXIT));
  printf("> ");
  return read_number_in_range(1, 4, lang);
}

int input_confirm_exit(int lang) {
  printf("%s\n", msg_get(lang, MSG_CONFIRM_EXIT));
  printf("> ");
  return read_number_in_range(1, 2, lang) == 1;
}

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
      return 0;

    if (key == CFG_KEY_ENTER) {
      if (ui_state->input_length > 0) {
        if (input_parse_move(ui_state->input_text, game->board_size, out_row,
                             out_col)) {
          ui_move_cursor(ui_state, *out_row, *out_col, game->board_size);
          input_clear(ui_state);
          return 1;
        }
        now_ms = GetTickCount64();
        ui_set_message(ui_state, MSG_INVALID_INPUT, 1, now_ms,
                       CFG_MESSAGE_HOLD_MS);
        continue;
      }
      *out_row = ui_state->cursor_row;
      *out_col = ui_state->cursor_col;
      return 1;
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
