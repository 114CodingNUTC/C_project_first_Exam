// 困難模式
int minimax(int depth, int alpha, int beta, int isMaximizing) {
  if (depth == 0 || checkWin())
    return evaluateBoard();

  if (isMaximizing) {
    int maxEval = -1000000;
    for (每一個空位) {
      board[r][c] = AI;
      int eval = minimax(depth - 1, alpha, beta, 0);
      board[r][c] = EMPTY; // 撤銷落子
      maxEval = max(maxEval, eval);
      alpha = max(alpha, eval);
      if (beta <= alpha)
        break; // 剪枝
    }
    return maxEval;
  } else {
    // 模擬玩家的最佳應對 (Minimizing)
    // ... 邏輯類似，取最小評分 ...
  }
}
