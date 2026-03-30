// 轉換難度
void aiMove() {
  if (difficulty == 1) {
    aiMoveNormal(); // 簡單隨機
  } else {
    // 困難模式：呼叫 Minimax 找到最佳座標 (bestR, bestC)
    int bestScore = -1000000;
    int bestR, bestC;
    // 遍歷所有空位，對每個點跑 Minimax
    // ...
    board[bestR][bestC] = 2;
    if (checkWin(bestR, bestC, 2))
      printf("AI (困難) 獲勝！\n");
  }
}
