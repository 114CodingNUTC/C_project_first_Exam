#include "config.h"
#include "events.h"
#include "messages.h"

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
