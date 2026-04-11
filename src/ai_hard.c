#include "../include/config.h"
#include "../include/functions.h"
#include <stdlib.h>

/*
 * ai_hard.c - 困難 AI 演算法說明
 *
 * 整體策略是「強制手優先 + 候選點縮減 + Minimax/Alpha-Beta」：
 * 1) 先處理立即勝負（我方致勝、對手致勝防守）。
 * 2) 再處理中短期戰術（延伸三連、阻擋對手三連轉四連）。
 * 3) 只對候選點做排序與搜尋，避免全盤展開。
 * 4) Minimax 使用 Alpha-Beta 剪枝，並套用分支上限與節點預算。
 * 5) 葉節點評分採棋形分 + 中心偏好 + 攻擊潛力。
 *
 * 目的：在固定計算成本內，取得比中等 AI 更穩定的攻防判斷品質。
 */

typedef struct HardMove {
  int row;
  int col;
  int score;
} HardMove;

/**
 * @brief 評估單一候選位置的分數（改進版）。
 * @param game 當前遊戲狀態。
 * @param row 候選列索引。
 * @param col 候選行索引。
 * @param player 評估玩家。
 * @return 該位置分數。
 */
static int eval_position(const GomokuGame *game, int row, int col, int player) {
  int total;
  int opposite;
  int i;
  int fwd;
  int bwd;
  int line_len;
  int directions[4][2];
  int both_sides_open;

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
    fwd = ai_count_line(game, row, col, player, directions[i][0],
                        directions[i][1]);
    bwd = ai_count_line(game, row, col, player, -directions[i][0],
                        -directions[i][1]);
    line_len = fwd + bwd + 1;

    /* Check if both sides are open (not blocked) */
    both_sides_open = 1;
    if (board_is_in_bounds(game, row - directions[i][0],
                           col - directions[i][1])) {
      if (game->board[row - directions[i][0]][col - directions[i][1]] ==
          opposite) {
        both_sides_open = 0;
      }
    }
    if (board_is_in_bounds(game, row + directions[i][0],
                           col + directions[i][1])) {
      if (game->board[row + directions[i][0]][col + directions[i][1]] ==
          opposite) {
        both_sides_open = 0;
      }
    }

    if (line_len >= CFG_WIN_STREAK) {
      total += CFG_AI_EVAL_WIN;
    } else if (line_len == 4) {
      total += CFG_AI_EVAL_FOUR;
      if (both_sides_open)
        total += CFG_AI_EVAL_FOUR / 2;
    } else if (line_len == 3) {
      total += CFG_AI_EVAL_THREE;
      if (both_sides_open)
        total += CFG_AI_EVAL_THREE / 2;
    } else if (line_len == 2) {
      total += CFG_AI_EVAL_TWO;
      if (both_sides_open)
        total += CFG_AI_EVAL_TWO / 2;
    } else if (line_len == 1) {
      total += CFG_AI_EVAL_ONE;
    }
  }

  /* Subtract opponent's potential (higher penalty for blocked threats) */
  for (i = 0; i < 4; i++) {
    fwd = ai_count_line(game, row, col, opposite, directions[i][0],
                        directions[i][1]);
    bwd = ai_count_line(game, row, col, opposite, -directions[i][0],
                        -directions[i][1]);
    line_len = fwd + bwd + 1;

    /* Check if opponent's sides are open */
    both_sides_open = 1;
    if (board_is_in_bounds(game, row - directions[i][0],
                           col - directions[i][1])) {
      if (game->board[row - directions[i][0]][col - directions[i][1]] == player)
        both_sides_open = 0;
    }
    if (board_is_in_bounds(game, row + directions[i][0],
                           col + directions[i][1])) {
      if (game->board[row + directions[i][0]][col + directions[i][1]] == player)
        both_sides_open = 0;
    }

    if (line_len >= CFG_WIN_STREAK) {
      total -= CFG_AI_EVAL_WIN * 3;
    } else if (line_len == 4) {
      total -= CFG_AI_EVAL_FOUR * 2;
      if (both_sides_open)
        total -= CFG_AI_EVAL_FOUR;
    } else if (line_len == 3) {
      total -= CFG_AI_EVAL_THREE * 2;
      if (both_sides_open)
        total -= CFG_AI_EVAL_THREE / 2;
    } else if (line_len == 2) {
      total -= CFG_AI_EVAL_TWO;
    }
  }

  return total;
}

/**
 * @brief 估算候選點集合的整體盤面分數。
 * @param game 當前遊戲狀態。
 * @param candidates 候選點矩陣。
 * @return 累積評估分數。
 */
static int
evaluate_board(const GomokuGame *game,
               int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE]) {
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

/**
 * @brief 給靠近中心的候選點額外加分。
 */
static int center_bias(const GomokuGame *game, int row, int col) {
  int center;

  center = game->board_size / 2;
  return CFG_AI_HARD_CENTER_BONUS - (abs(row - center) + abs(col - center));
}

/**
 * @brief 計算某方向連線的開口端數量（0~2）。
 */
static int count_open_ends(const GomokuGame *game, int row, int col, int player,
                           int dr, int dc) {
  int open_ends;
  int r;
  int c;

  open_ends = 0;

  r = row + dr;
  c = col + dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    r += dr;
    c += dc;
  }
  if (board_is_in_bounds(game, r, c) && game->board[r][c] == STONE_EMPTY)
    open_ends++;

  r = row - dr;
  c = col - dc;
  while (board_is_in_bounds(game, r, c) && game->board[r][c] == player) {
    r -= dr;
    c -= dc;
  }
  if (board_is_in_bounds(game, r, c) && game->board[r][c] == STONE_EMPTY)
    open_ends++;

  return open_ends;
}

/**
 * @brief 評估落在指定位置後的進攻潛力（雙四、雙活三等）。
 */
static int evaluate_attack_potential(const GomokuGame *game, int row, int col,
                                     int player) {
  int dirs[4][2];
  int i;
  int len;
  int open_ends;
  int threat_four;
  int threat_open_three;
  int score;
  GomokuGame temp;

  if (!board_is_empty(game, row, col))
    return CFG_AI_SCORE_NEG_INF;

  temp = *game;
  board_place_stone(&temp, player, row, col);

  dirs[0][0] = 0;
  dirs[0][1] = 1;
  dirs[1][0] = 1;
  dirs[1][1] = 0;
  dirs[2][0] = 1;
  dirs[2][1] = 1;
  dirs[3][0] = 1;
  dirs[3][1] = -1;

  score = 0;
  threat_four = 0;
  threat_open_three = 0;
  for (i = 0; i < 4; i++) {
    len = 1 + ai_count_line(&temp, row, col, player, dirs[i][0], dirs[i][1]) +
          ai_count_line(&temp, row, col, player, -dirs[i][0], -dirs[i][1]);
    open_ends =
        count_open_ends(&temp, row, col, player, dirs[i][0], dirs[i][1]);

    if (len >= CFG_WIN_STREAK) {
      score += CFG_AI_EVAL_WIN;
      continue;
    }

    if (len == CFG_WIN_STREAK - 1 && open_ends > 0) {
      threat_four++;
      score += CFG_AI_EVAL_FOUR;
    } else if (len == CFG_WIN_STREAK - 2 && open_ends == 2) {
      threat_open_three++;
      score += CFG_AI_EVAL_THREE;
    } else if (len == CFG_WIN_STREAK - 2 && open_ends == 1) {
      score += CFG_AI_EVAL_TWO;
    }
  }

  if (threat_four >= 2)
    score += CFG_AI_EVAL_WIN / 2;
  if (threat_open_three >= 2)
    score += CFG_AI_EVAL_FOUR;

  return score;
}

/**
 * @brief 從候選點中挑選前 N 個高分步，供 Minimax 擴展。
 */
static int
collect_top_moves(const GomokuGame *game,
                  int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE],
                  int player, int max_moves, HardMove *out_moves) {
  int row;
  int col;
  int count;
  int score;
  int i;

  count = 0;
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      if (!candidates[row][col] || !board_is_empty(game, row, col))
        continue;

      score = eval_position(game, row, col, player);
      score += center_bias(game, row, col);
      score += evaluate_attack_potential(game, row, col, player);

      if (count < max_moves) {
        i = count;
        count++;
      } else if (out_moves[max_moves - 1].score < score) {
        i = max_moves - 1;
      } else {
        continue;
      }

      while (i > 0 && out_moves[i - 1].score < score) {
        out_moves[i] = out_moves[i - 1];
        i--;
      }
      out_moves[i].row = row;
      out_moves[i].col = col;
      out_moves[i].score = score;
    }
  }

  return count;
}

/**
 * @brief Minimax + Alpha-Beta 搜尋。
 * @param game 可變遊戲狀態副本。
 * @param depth 剩餘搜尋深度。
 * @param alpha Alpha 下界。
 * @param beta Beta 上界。
 * @param maximizing 是否為最大化節點。
 * @param candidates 候選點矩陣。
 * @return 此節點評估分數。
 */
static int minimax(GomokuGame *game, int depth, int alpha, int beta,
                   int maximizing,
                   int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE],
                   int *node_budget) {
  int i;
  int score;
  int best_score;
  int win_count;
  int orig_player;
  int out_line[CFG_WIN_STREAK][2];
  int move_count;
  int player_for_order;
  int branch_limit;
  HardMove moves[CFG_AI_HARD_BRANCH_LIMIT_ROOT];

  if (*node_budget <= 0)
    return evaluate_board(game, candidates);

  (*node_budget)--;
  if (depth == 0)
    return evaluate_board(game, candidates);

  if (depth == CFG_AI_HARD_DEPTH - 1)
    branch_limit = CFG_AI_HARD_BRANCH_LIMIT_ROOT;
  else
    branch_limit = CFG_AI_HARD_BRANCH_LIMIT_NODE;

  if (maximizing) {
    player_for_order = game->current_player;
  } else {
    player_for_order =
        (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
  }
  move_count = collect_top_moves(game, candidates, player_for_order,
                                 branch_limit, moves);

  if (maximizing) {
    best_score = CFG_AI_SCORE_NEG_INF;
    for (i = 0; i < move_count; i++) {
      board_place_stone(game, game->current_player, moves[i].row, moves[i].col);

      if (rules_check_win(game, moves[i].row, moves[i].col,
                          game->current_player, out_line, &win_count)) {
        score = CFG_AI_EVAL_WIN;
      } else {
        orig_player = game->current_player;
        game->current_player =
            (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
        score =
            minimax(game, depth - 1, alpha, beta, 0, candidates, node_budget);
        game->current_player = orig_player;
      }

      game->board[moves[i].row][moves[i].col] = STONE_EMPTY;

      if (score > best_score)
        best_score = score;
      if (score > alpha)
        alpha = score;
      if (beta <= alpha || *node_budget <= 0)
        break;
    }
    return (best_score == CFG_AI_SCORE_NEG_INF)
               ? evaluate_board(game, candidates)
               : best_score;
  } else {
    best_score = CFG_AI_SCORE_POS_INF;
    for (i = 0; i < move_count; i++) {
      board_place_stone(game,
                        (game->current_player == STONE_BLACK) ? STONE_WHITE
                                                              : STONE_BLACK,
                        moves[i].row, moves[i].col);

      orig_player = game->current_player;
      game->current_player =
          (game->current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
      score = minimax(game, depth - 1, alpha, beta, 1, candidates, node_budget);
      game->current_player = orig_player;

      game->board[moves[i].row][moves[i].col] = STONE_EMPTY;

      if (score < best_score)
        best_score = score;
      if (score < beta)
        beta = score;
      if (beta <= alpha || *node_budget <= 0)
        break;
    }
    return (best_score == CFG_AI_SCORE_POS_INF)
               ? evaluate_board(game, candidates)
               : best_score;
  }
}

/**
 * @brief 建立候選落子點集合，縮小搜尋範圍。
 * @param game 當前遊戲狀態。
 * @param candidates 候選點矩陣輸出。
 */
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

/**
 * @brief 困難 AI：先做立即勝負檢查，再用深度 Minimax + Alpha-Beta 搜尋。
 * @param game 當前遊戲狀態。
 * @param out_row 輸出列索引。
 * @param out_col 輸出行索引。
 * @return 1 代表成功找到位置；0 代表無合法位置或參數錯誤。
 */
int ai_hard_pick_move(const GomokuGame *game, int *out_row, int *out_col) {
  int move_idx;
  int score;
  int best_score;
  int best_row;
  int best_col;
  int candidates[CFG_MAX_BOARD_SIZE][CFG_MAX_BOARD_SIZE];
  GomokuGame game_copy;
  int out_line[CFG_WIN_STREAK][2];
  int win_count;
  int search_depth;
  int node_budget;
  int budget_for_move;
  int move_count;
  int attack_bonus;
  HardMove moves[CFG_AI_HARD_BRANCH_LIMIT_ROOT];

  if (game == 0 || out_row == 0 || out_col == 0)
    return 0;

  if (ai_try_forced_or_tactical_move(game, out_row, out_col))
    return 1;

  find_candidates(game, candidates);

  best_score = CFG_AI_SCORE_NEG_INF;
  best_row = -1;
  best_col = -1;

  /* Increase search depth based on game phase */
  search_depth = CFG_AI_HARD_DEPTH;
  if (game->move_count < 10)
    search_depth = CFG_AI_HARD_DEPTH - 1;
  else if (game->move_count > 20)
    search_depth = CFG_AI_HARD_DEPTH;

  if (game->move_count < 10)
    node_budget = CFG_AI_HARD_NODE_BUDGET_EARLY;
  else if (game->move_count < 30)
    node_budget = CFG_AI_HARD_NODE_BUDGET_MID;
  else
    node_budget = CFG_AI_HARD_NODE_BUDGET_LATE;

  move_count = collect_top_moves(game, candidates, game->current_player,
                                 CFG_AI_HARD_BRANCH_LIMIT_ROOT, moves);

  /* Try top candidate positions with enhanced minimax */
  for (move_idx = 0; move_idx < move_count; move_idx++) {
    /* Copy game state */
    game_copy = *game;

    /* Try this move */
    board_place_stone(&game_copy, game->current_player, moves[move_idx].row,
                      moves[move_idx].col);

    /* Check if it wins immediately */
    if (rules_check_win(&game_copy, moves[move_idx].row, moves[move_idx].col,
                        game->current_player, out_line, &win_count)) {
      score = CFG_AI_EVAL_WIN + 1000;
    } else {
      /* Run enhanced minimax with adaptive depth */
      game_copy.current_player =
          (game_copy.current_player == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
      budget_for_move = node_budget;
      score = minimax(&game_copy, search_depth - 1, CFG_AI_SCORE_NEG_INF,
                      CFG_AI_SCORE_POS_INF, 0, candidates, &budget_for_move);

      attack_bonus = evaluate_attack_potential(
          game, moves[move_idx].row, moves[move_idx].col, game->current_player);
      if (attack_bonus > 0)
        score += attack_bonus;
    }

    if (score > best_score) {
      best_score = score;
      best_row = moves[move_idx].row;
      best_col = moves[move_idx].col;
    }
  }

  if (best_row == -1)
    return ai_medium_pick_move(game, out_row, out_col);

  *out_row = best_row;
  *out_col = best_col;
  return 1;
}
