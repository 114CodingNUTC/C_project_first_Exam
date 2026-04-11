#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "config.h"

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
int game_surrender(GomokuGame *game, int player, int *out_event_code,
                   int *out_message_key);
int game_run_loop(int mode, int board_size, int ai_turn_choice, int lang);

int input_parse_move(const char *text, int board_size, int *out_row,
                     int *out_col);
int input_read_player_move(const GomokuGame *game, UIState *ui_state,
                           int *out_row, int *out_col);

/* 新式菜單選擇函數 (視覺化選單) */
int input_choose_mode_visual(int *lang);
int input_choose_board_size_visual(int *lang);
int input_choose_ai_turn_visual(int *lang);
int input_choose_pause_action_visual(int *lang);
int input_confirm_exit_visual(int *lang);

/* UI 顏色與光標控制 */
void ui_set_color(int color);
void ui_gotoxy(int x, int y);
void ui_hide_cursor(void);

/* UI 選單繪製 */
void ui_draw_menu_title(int lang);
void ui_draw_menu_options(int current_selection, int option_count,
                          const char **option_texts);
void ui_draw_menu_hint(const char *hint_text);

void ui_init_state(UIState *state, int board_size);
void ui_render_full(const GomokuGame *game, const UIState *state, int lang);
void ui_set_message(UIState *state, int message_key, int is_error,
                    long long current_ms, long long hold_ms);
int ui_message_expired(const UIState *state, long long current_ms);
void ui_move_cursor(UIState *state, int row, int col, int board_size);

int ai_easy_pick_move(const GomokuGame *game, int *out_row, int *out_col);
int ai_medium_pick_move(const GomokuGame *game, int *out_row, int *out_col);
int ai_hard_pick_move(const GomokuGame *game, int *out_row, int *out_col);

int ai_count_line(const GomokuGame *game, int row, int col, int player, int dr,
                  int dc);
int ai_find_immediate_move(const GomokuGame *game, int player, int *out_row,
                           int *out_col);
int ai_find_block_four_threat_move(const GomokuGame *game, int player,
                                   int *out_row, int *out_col);
int ai_find_extend_three_move(const GomokuGame *game, int player, int *out_row,
                              int *out_col);
int ai_has_near_stone(const GomokuGame *game, int row, int col, int range);
int ai_try_forced_or_tactical_move(const GomokuGame *game, int *out_row,
                                   int *out_col);

const char *msg_get(int lang, int key);

#endif
