int main() {
  printf("請輸入棋盤大小：");
  if (scanf("%d", &boardSize) != 1) {
    printf("輸入錯誤！");
    return 1;
  }

  // 取得 boardSize 後，再進行動態記憶體分配
  initBoard(boardSize);

  // ... 進入遊戲迴圈 ...
}
