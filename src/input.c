#include "config.h"
#include <ctype.h>
#include <stdlib.h>

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
