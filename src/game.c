#include "config.h"
#include "events.h"
#include "messages.h"
#include <conio.h>
#include <stdio.h>
#include <windows.h>

static long long now_ms(void) { return GetTickCount64(); }

static int handle_ai_turn(const GomokuGame *game, int mode, int *out_row,
                          int *out_col) {
  if (mode == 2)
    return ai_easy_pick_move(game, out_row, out_col);
  if (mode == 3)
    return ai_medium_pick_move(game, out_row, out_col);
  if (mode == 4)
    return ai_hard_pick_move(game, out_row, out_col);
  return 0;
}

void game_reset(GomokuGame *game, int board_size) {
  int i;

  if (game == 0)
    return;

  board_init(game, board_size);
  game->current_player = STONE_BLACK;
  game->move_count = 0;
  game->last_row = -1;
  game->last_col = -1;
  game->game_over = 0;
  game->winner = STONE_EMPTY;
  game->win_line_count = 0;
  for (i = 0; i < 5; i++) {
    game->win_line[i][0] = -1;
    game->win_line[i][1] = -1;
  }
}

static void set_output(int *out_event_code, int *out_message_key,
                       int event_code, int msg_key) {
  if (out_event_code != 0)
    *out_event_code = event_code;
  if (out_message_key != 0)
    *out_message_key = msg_key;
}

int try_place_stone(GomokuGame *game, int player, int row, int col,
                    int *out_event_code, int *out_message_key) {
  int line_count;

  if (game == 0) {
    set_output(out_event_code, out_message_key, EV_ERROR, MSG_INTERNAL_ERROR);
    return PLACE_ERR_INVALID_ARG;
  }
  if (player != STONE_BLACK && player != STONE_WHITE) {
    set_output(out_event_code, out_message_key, EV_ERROR, MSG_INVALID_INPUT);
    return PLACE_ERR_INVALID_ARG;
  }
  if (game->game_over) {
    set_output(out_event_code, out_message_key, EV_INVALID_MOVE, MSG_GAME_OVER);
    return PLACE_ERR_GAME_OVER;
  }
  if (player != game->current_player) {
    set_output(out_event_code, out_message_key, EV_INVALID_MOVE,
               MSG_NOT_YOUR_TURN);
    return PLACE_ERR_NOT_CURRENT_PLAYER;
  }
  if (!board_is_in_bounds(game, row, col)) {
    set_output(out_event_code, out_message_key, EV_INVALID_MOVE,
               MSG_OUT_OF_BOUNDS);
    return PLACE_ERR_OUT_OF_BOUNDS;
  }
  if (!board_is_empty(game, row, col)) {
    set_output(out_event_code, out_message_key, EV_INVALID_MOVE,
               MSG_CELL_OCCUPIED);
    return PLACE_ERR_OCCUPIED;
  }
  if (!board_place_stone(game, player, row, col)) {
    set_output(out_event_code, out_message_key, EV_ERROR, MSG_INTERNAL_ERROR);
    return PLACE_ERR_INTERNAL;
  }

  game->move_count++;
  game->last_row = row;
  game->last_col = col;

  line_count = 0;
  if (rules_check_win(game, row, col, player, game->win_line, &line_count)) {
    game->win_line_count = line_count;
    game->game_over = 1;
    game->winner = player;
    if (player == STONE_BLACK)
      set_output(out_event_code, out_message_key, EV_WIN, MSG_WIN_BLACK);
    else
      set_output(out_event_code, out_message_key, EV_WIN, MSG_WIN_WHITE);
    return PLACE_OK;
  }

  if (rules_is_draw(game)) {
    game->game_over = 1;
    game->winner = STONE_EMPTY;
    set_output(out_event_code, out_message_key, EV_DRAW, MSG_DRAW);
    return PLACE_OK;
  }

  game->current_player = (player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
  if (game->current_player == STONE_BLACK) {
    set_output(out_event_code, out_message_key, EV_STONE_PLACED,
               MSG_TURN_BLACK);
  } else {
    set_output(out_event_code, out_message_key, EV_STONE_PLACED,
               MSG_TURN_WHITE);
  }

  return PLACE_OK;
}

void game_run_loop(int mode, int board_size, int lang) {
  GomokuGame game;
  UIState ui_state;
  int row;
  int col;
  int event_code;
  int msg_key;
  int player;

  game_reset(&game, board_size);
  ui_init_state(&ui_state, board_size);
  ui_set_message(&ui_state, MSG_READY, 0, now_ms(), CFG_MESSAGE_HOLD_MS);

  while (!game.game_over) {
    if (mode != 1 && game.current_player == STONE_WHITE) {
      if (!handle_ai_turn(&game, mode, &row, &col))
        break;
    } else {
      if (!input_read_player_move(&game, &ui_state, &row, &col))
        break;
    }

    player = game.current_player;
    if (try_place_stone(&game, player, row, col, &event_code, &msg_key) !=
        PLACE_OK) {
      ui_set_message(&ui_state, msg_key, 1, now_ms(), CFG_MESSAGE_HOLD_MS);
      continue;
    }

    ui_set_message(&ui_state, msg_key, event_code == EV_INVALID_MOVE, now_ms(),
                   CFG_MESSAGE_HOLD_MS);
    ui_move_cursor(&ui_state, row, col, game.board_size);
  }

  ui_render_full(&game, &ui_state, lang);
  if (game.game_over) {
    if (game.winner == STONE_BLACK)
      ui_show_message(MSG_WIN_BLACK);
    else if (game.winner == STONE_WHITE)
      ui_show_message(MSG_WIN_WHITE);
    else
      ui_show_message(MSG_DRAW);
  }
  printf("按任意鍵返回主選單...\n");
  _getch();
}
