#include <stdio.h>
#include <stdlib.h>
#include <time.h> // 1. 引入時間函式庫

int main() {
  int choice;
  int player = 0;

  // 2. 初始化隨機種子（用當前時間作為基準）
  // 這行程式碼在整個遊戲執行過程中「只需要執行一次」
  srand((unsigned int)time(NULL));

  printf("---------- 五子棋：先後手設定 ----------\n");
  printf(" [1] 先手 (黑棋 x)\n");
  printf(" [2] 後手 (白棋 ○)\n");
  printf(" [3] 隨機決定 \n");
  printf("----------------------------------------\n");

  while (1) {
    printf("請輸入選擇 (1-3): ");
    if (scanf("%d", &choice) != 1) {
      printf("錯誤：請輸入數字！\n");
      while (getchar() != '\n')
        ;
      continue;
    }

    if (choice == 1) {
      player = 1;
      break;
    } else if (choice == 2) {
      player = 2;
      break;
    } else if (choice == 3) {
      // 現在這裡每次執行，結果都會不一樣了！
      player = (rand() % 2) + 1;
      printf(">> 系統隨機選定結果：【%s】\n", (player == 1) ? "先手" : "後手");
      break;
    } else {
      printf("錯誤：請輸入 1, 2 或 3。\n");
    }
  }

  printf("\n遊戲準備開始，你持%s。\n", (player == 1) ? "黑棋 x" : "白棋 ○");

  return 0;
}
