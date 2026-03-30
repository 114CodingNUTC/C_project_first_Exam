#include "../include/config.h"
#include <stdio.h>

#define BOARD_SIZE 15

int board[BOARD_SIZE][BOARD_SIZE];

// 初始化棋盤
void initBoard() {
  setup();
  for (int i = 0; i < BOARD_SIZE; i++)
    for (int j = 0; j < BOARD_SIZE; j++)
      board[i][j] = 0;
}

// 顯示棋盤
void displayBoard() {
  printf("\n");
  printf("     ");
  for (int i = 0; i < BOARD_SIZE; i++)
    printf("%2d ", i + 1);
  printf("\n");

  for (int i = 0; i < BOARD_SIZE; i++) {
    printf("%2d  ", i + 1);
    for (int j = 0; j < BOARD_SIZE; j++)
      printf(STONE_CHAR_EMPTY);
    printf("\n");
  }
  printf("\n");
}

// 主程序
int main() {
  initBoard();
  displayBoard();
  return 0;
}
