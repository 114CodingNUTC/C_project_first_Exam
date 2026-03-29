#ifndef CONFIG_H
#define CONFIG_H

#define CFG_BOARD_SIZE_9 9
#define CFG_BOARD_SIZE_15 15
#define CFG_BOARD_SIZE_19 19
#define CFG_BOARD_SIZE_DEFAULT CFG_BOARD_SIZE_15
#define CFG_MAX_BOARD_SIZE CFG_BOARD_SIZE_19

#define CFG_WIN_STREAK 5

#define CFG_MESSAGE_HOLD_MS 0x7D0
#define CFG_INPUT_BUFFER_SIZE 32

#define CFG_MESSAGE_SLOT_HEIGHT 3
#define CFG_MIN_DISPLAY_HEIGHT 3
#define CFG_CLEAR_SCREEN_CMD "cls"

#define CFG_CONSOLE_CODEPAGE_UTF8 0xFDE9

#define CURSOR_HIDDEN 0
#define CURSOR_VISIBLE 1
#define CFG_CONSOLE_OUTPUT_CODEPAGE CFG_CONSOLE_CODEPAGE_UTF8
#define CFG_CONSOLE_INPUT_CODEPAGE CFG_CONSOLE_CODEPAGE_UTF8

#define CFG_ANSI_CLEAR_AND_HOME "\x1b[3J\x1b[2J\x1b[H"
#define CFG_ANSI_COLOR_RED "\x1b[31m"
#define CFG_ANSI_COLOR_RESET "\x1b[0m"

#define LANG_ZH_TW 0
#define LANG_EN_US 1
#define LANG_DEFAULT LANG_ZH_TW

#define CFG_KEY_CTRL_C 3
#define CFG_KEY_BACKSPACE 8
#define CFG_KEY_ENTER 13
#define CFG_KEY_ESC 27
#define CFG_KEY_EXTENDED_0 0
#define CFG_KEY_EXTENDED_224 224
#define CFG_KEY_DELETE 83
#define CFG_KEY_UP 72
#define CFG_KEY_DOWN 80
#define CFG_KEY_LEFT 75
#define CFG_KEY_RIGHT 77

#define CFG_AI_MEDIUM_NEAR_RANGE 2

#define CFG_AI_HARD_DEPTH 5
#define CFG_AI_HARD_SEARCH_RANGE 2
#define CFG_AI_EVAL_WIN 0x186A0
#define CFG_AI_EVAL_FOUR 0x2710
#define CFG_AI_EVAL_THREE 0x3E8
#define CFG_AI_EVAL_TWO 0x64
#define CFG_AI_EVAL_ONE 0xA
#define CFG_AI_SCORE_POS_INF 0xF4240
#define CFG_AI_SCORE_NEG_INF (-CFG_AI_SCORE_POS_INF)

#define STONE_EMPTY 0
#define STONE_BLACK 1
#define STONE_WHITE 2

#define CFG_STONE_CHAR_BLACK 'X'
#define CFG_STONE_CHAR_WHITE 'O'
#define CFG_STONE_CHAR_EMPTY '.'
#define CFG_STONE_CHAR_BLACK_LAST 'x'
#define CFG_STONE_CHAR_WHITE_LAST 'o'
#define CFG_STONE_CHAR_WIN_FALLBACK '#'

#define CFG_AI_TURN_PLAYER_FIRST 1
#define CFG_AI_TURN_AI_FIRST 2

typedef struct GomokuGame {
  int board_size;
  int board[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE];
  int current_player;
  int move_count;
  int last_row;
  int last_col;
  int game_over;
  int winner;
  int win_line[CFG_WIN_STREAK][2];
  int win_line_count;
} GomokuGame;

typedef struct UIState {
  int cursor_row;
  int cursor_col;
  int message_key;
  long long message_display_end_ms;
  int message_is_error;
  int board_dirty;
  char input_text[CFG_INPUT_BUFFER_SIZE];
  int input_length;
  int input_cursor;
} UIState;

#endif
