#include "config.h"
#include <stdlib.h>

static int count_line(const GomokuGame *game, int row, int col, int player,
                      int dr, int dc) {
  int count;
  int r;
  int c;

  count = 0;
  r = row + dr;
  c = col + dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    count++;
    r += dr;
    c += dc;
  }
  return count;
}

static int eval_position(const GomokuGame *game, int row, int col, int player) {
  int total;
  int opposite;
  int i;
  int fwd;
  int bwd;
  int line_len;
  int directions[4][2];

  opposite = (player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

  directions[0][0] = 0;
  directions[0][1] = 1;
  directions[1][0] = 1;
  directions[1][1] = 0;
  directions[2][0] = 1;
  directions[2][1] = 1;
  directions[3][0] = 1;
  directions[3][1] = -1;

  total = 0;
  for (i = 0; i < 4; i++) {
    /* Count in forward and backward direction */
    fwd =
        count_line(game, row, col, player, directions[i][0], directions[i][1]);
    bwd = count_line(game, row, col, player, -directions[i][0],
                     -directions[i][1]);
    line_len = fwd + bwd + 1;

    if (line_len >= CFG_WIN_STREAK)
      total += CFG_AI_EVAL_WIN;
    else if (line_len == 4)
      total += CFG_AI_EVAL_FOUR;
    else if (line_len == 3)
      total += CFG_AI_EVAL_THREE;
    else if (line_len == 2)
      total += CFG_AI_EVAL_TWO;
    else if (line_len == 1)
      total += CFG_AI_EVAL_ONE;
  }

  /* Subtract opponent's potential (negative for maximizing our score) */
  for (i = 0; i < 4; i++) {
    fwd = count_line(game, row, col, opposite, directions[i][0],
                     directions[i][1]);
    bwd = count_line(game, row, col, opposite, -directions[i][0],
                     -directions[i][1]);
    line_len = fwd + bwd + 1;

    if (line_len >= CFG_WIN_STREAK)
      total -= CFG_AI_EVAL_WIN * 2;
    else if (line_len == 4)
      total -= CFG_AI_EVAL_FOUR * 2;
    else if (line_len == 3)
      total -= CFG_AI_EVAL_THREE;
    else if (line_len == 2)
      total -= CFG_AI_EVAL_TWO;
  }

  return total;
}

static int
evaluate_board(const GomokuGame *game,
               const int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE]) {
  int row;
  int col;
  int score;
  int total;

  total = 0;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (candidates[row][col]) {
        score = eval_position(game, row, col, game->current_player);
        if (score > 0)
          total += score;
      }
    }
  }

  return total;
}

static int minimax(GomokuGame *game, int depth, int alpha, int beta,
                   int maximizing,
                   int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE]) {
  int row;
  int col;
  int score;
  int best_score;
  int out_event;
  int orig_player;
  int out_line[CFG_WIN_STREAK][2];

  if (depth == 0)
    return evaluate_board(game, candidates);

  if (maximizing) {
    best_score = CFG_AI_SCORE_NEG_INF;
    for (row = 0; row < game->board_size; row++) {
      for (col = 0; col < game->board_size; col++) {
        if (candidates[row][col] && board_is_empty(game, row, col)) {
          board_place_stone(game, game->current_player, row, col);

          if (rules_check_win(game, row, col, game->current_player, out_line,
                              &out_event)) {
            score = CFG_AI_EVAL_WIN;
          } else {
            orig_player = game->current_player;
            game->current_player = (game->current_player == STONE_BLACK)
                                       ? STONE_WHITE
                                       : STONE_BLACK;
            score = minimax(game, depth - 1, alpha, beta, 0, candidates);
            game->current_player = orig_player;
          }

          game->board[row][col] = STONE_EMPTY;

          if (score > best_score)
            best_score = score;
          if (score > alpha)
            alpha = score;
          if (beta <= alpha)
            break;
        }
      }
      if (beta <= alpha)
        break;
    }
    return (best_score == CFG_AI_SCORE_NEG_INF)
               ? evaluate_board(game, candidates)
               : best_score;
  } else {
    best_score = CFG_AI_SCORE_POS_INF;
    for (row = 0; row < game->board_size; row++) {
      for (col = 0; col < game->board_size; col++) {
        if (candidates[row][col] && board_is_empty(game, row, col)) {
          board_place_stone(game,
                            (game->current_player == STONE_BLACK) ? STONE_WHITE
                                                                  : STONE_BLACK,
                            row, col);

          orig_player = game->current_player;
          game->current_player =
              (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
          score = minimax(game, depth - 1, alpha, beta, 1, candidates);
          game->current_player = orig_player;

          game->board[row][col] = STONE_EMPTY;

          if (score < best_score)
            best_score = score;
          if (score < beta)
            beta = score;
          if (beta <= alpha)
            break;
        }
      }
      if (beta <= alpha)
        break;
    }
    return (best_score == CFG_AI_SCORE_POS_INF)
               ? evaluate_board(game, candidates)
               : best_score;
  }
}

static void
find_candidates(const GomokuGame *game,
                int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE]) {
  int row;
  int col;
  int r;
  int c;
  int range;

  range = CFG_AI_HARD_SEARCH_RANGE;

  for (row = 0; row < game->board_size; row++)
    for (col = 0; col < game->board_size; col++)
      candidates[row][col] = 0;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (game->board[row][col] != STONE_EMPTY) {
        for (r = row - range; r <= row + range; r++) {
          for (c = col - range; c <= col + range; c++)
            if (board_is_in_bounds(game, r, c))
              candidates[r][c] = 1;
        }
      }
    }
  }

  /* If board is empty, allow center */
  if (game->move_count == 0) {
    int center;
    center = game->board_size / 2;
    candidates[center][center] = 1;
  }
}

static int find_immediate_move(const GomokuGame *game, int player, int *out_row,
                               int *out_col) {
  int row;
  int col;
  int out_count;
  int out_line[CFG_WIN_STREAK][2];
  GomokuGame temp;

  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!board_is_empty(game, row, col))
        continue;

      temp = *game;
      board_place_stone(&temp, player, row, col);
      if (rules_check_win(&temp, row, col, player, out_line, &out_count)) {
        *out_row = row;
        *out_col = col;
        return 1;
      }
    }
  }

  return 0;
}

int ai_hard_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int row;
  int col;
  int score;
  int best_score;
  int best_row;
  int best_col;
  int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE];
  GomokuGame game_copy;
  int out_line[CFG_WIN_STREAK][2];
  int out_event;
  int opponent;

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  opponent = (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;

  if (find_immediate_move(game, game->current_player, out_row, out_col))
    return 1;

  if (find_immediate_move(game, opponent, out_row, out_col))
    return 1;

  find_candidates(game, candidates);

  best_score = CFG_AI_SCORE_NEG_INF;
  best_row = -1;
  best_col = -1;

  /* Try each candidate position */
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (candidates[row][col] && board_is_empty(game, row, col)) {
        /* Copy game state */
        game_copy = *game;

        /* Try this move */
        board_place_stone(&game_copy, game->current_player, row, col);

        /* Check if it wins immediately */
        if (rules_check_win(&game_copy, row, col, game->current_player,
                            out_line, &out_event)) {
          score = CFG_AI_EVAL_WIN;
        } else {
          /* Run minimax */
          game_copy.current_player = (game_copy.current_player == STONE_BLACK)
                                         ? STONE_WHITE
                                         : STONE_BLACK;
          score =
              minimax(&game_copy, CFG_AI_HARD_DEPTH - 1, CFG_AI_SCORE_NEG_INF,
                      CFG_AI_SCORE_POS_INF, 0, candidates);
        }

        if (score > best_score) {
          best_score = score;
          best_row = row;
          best_col = col;
        }
      }
    }
  }

  if (best_row == -1)
    return ai_medium_pick_move(game, out_row, out_col);

  *out_row = best_row;
  *out_col = best_col;
  return 1;
}
