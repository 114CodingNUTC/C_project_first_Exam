#include "config.h"
#include "messages.h"
#include <stdio.h>

static void print_result(int *index, const char *name, int ok) {
  (*index)++;
  printf("%s Test %d: %s\n", ok ? "PASS" : "FAIL", *index, name);
}

int main(void) {
  int t;
  int row;
  int col;
  UIState state;

  t = 0;

  print_result(&t, "parse a1",
               input_parse_move("a1", 15, &row, &col) && row == 0 && col == 0);
  print_result(&t, "parse A,10",
               input_parse_move("A,10", 15, &row, &col) && row == 9 &&
                   col == 0);
  print_result(&t, "parse 10a",
               input_parse_move("10a", 15, &row, &col) && row == 9 && col == 0);
  print_result(&t, "reject out of bounds",
               !input_parse_move("z1", 15, &row, &col));
  print_result(&t, "reject bad format",
               !input_parse_move("@@", 15, &row, &col));

  ui_init_state(&state, 15);
  print_result(&t, "cursor init center",
               state.cursor_row == 7 && state.cursor_col == 7);

  ui_move_cursor(&state, -5, 99, 15);
  print_result(&t, "cursor clamp",
               state.cursor_row == 0 && state.cursor_col == 14);

  ui_set_message(&state, MSG_INVALID_INPUT, 1, 1000, 2000);
  print_result(&t, "message set",
               state.message_key == MSG_INVALID_INPUT &&
                   state.message_is_error == 1);
  print_result(&t, "message not expired", !ui_message_expired(&state, 2500));
  print_result(&t, "message expired", ui_message_expired(&state, 3000));

  printf("Total: %d\n", t);
  return 0;
}
