// 普通模式
void aiMoveNormal() {
  // 1. 遍歷棋盤找玩家即將連成五子的點 (防禦)
  // 2. 遍歷棋盤找自己即將連成五子的點 (進攻)
  // 3. 若都沒有，則隨機落子
  int r, c;
  do {
    r = rand() % boardSize;
    c = rand() % boardSize;
  } while (board[r][c] != 0);
  board[r][c] = 2; // AI 落子
}
