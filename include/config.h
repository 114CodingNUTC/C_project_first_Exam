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

void board_init(GomokuGame *game, int board_size);
int board_is_in_bounds(const GomokuGame *game, int row, int col);
int board_is_empty(const GomokuGame *game, int row, int col);
int board_place_stone(GomokuGame *game, int player, int row, int col);

int rules_check_win(const GomokuGame *game, int row, int col, int player,
                    int out_line[CFG_WIN_STREAK][2], int *out_count);
int rules_is_draw(const GomokuGame *game);

void game_reset(GomokuGame *game, int board_size);
int try_place_stone(GomokuGame *game, int player, int row, int col,
                    int *out_event_code, int *out_message_key);
int game_run_loop(int mode, int board_size, int ai_turn_choice, int lang);

int input_parse_move(const char *text, int board_size, int *out_row,
                     int *out_col);
int input_read_player_move(const GomokuGame *game, UIState *ui_state,
                           int *out_row, int *out_col);
int input_choose_mode(int lang);
int input_choose_board_size(int lang);
int input_choose_ai_turn(int lang);
int input_choose_pause_action(int lang);
int input_confirm_exit(int lang);

void ui_render_board(const GomokuGame *game);
void ui_show_message(int message_key);

void ui_init_state(UIState *state, int board_size);
void ui_render_full(const GomokuGame *game, const UIState *state, int lang);
void ui_set_message(UIState *state, int message_key, int is_error,
                    long long current_ms, long long hold_ms);
int ui_message_expired(const UIState *state, long long current_ms);
void ui_move_cursor(UIState *state, int row, int col, int board_size);

int ai_easy_pick_move(const GomokuGame *game, int *out_row, int *out_col);
int ai_medium_pick_move(const GomokuGame *game, int *out_row, int *out_col);
int ai_hard_pick_move(const GomokuGame *game, int *out_row, int *out_col);

#endif
