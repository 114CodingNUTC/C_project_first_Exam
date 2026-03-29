#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY 0
#define PLAYER1 1 // 黑子 (X)
#define PLAYER2 2 // 白子 (O)

int **board;
int boardSize;
int gameMode; // 1: 人人, 2: 人機

// 函數宣告
void initBoard(int size);
void printBoard();
int checkWin(int r, int c, int player);
void playerMove(int player);
void aiMove();
void freeBoard();

int main() {
  int r, c, currentPlayer = PLAYER1;
  int totalMoves = 0;

  printf("--- 歡迎進入五子棋遊戲 ---\n");
  printf("請輸入棋盤大小 (例如 15): ");
  scanf("%d", &boardSize);
  initBoard(boardSize);

  printf("選擇模式: (1)人人對戰 (2)人機對戰: ");
  scanf("%d", &gameMode);

  while (1) {
    system("cls"); // 清除畫面 (Dev C++ 適用)
    printBoard();

    if (currentPlayer == PLAYER1 || gameMode == 1) {
      playerMove(currentPlayer);
    } else {
      printf("AI 正在思考...\n");
      aiMove();
    }

    totalMoves++;
    // 這裡簡化檢查，實際需傳入最後落子位置
    // 為了教學清楚，我們假設在每次落子後檢查全盤（或傳入座標）

    // 檢查勝負邏輯... (此處省略部分重複代碼，詳見下方 checkWin 使用)

    // 切換玩家
    currentPlayer = (currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;

    if (totalMoves >= boardSize * boardSize) {
      printf("平手！\n");
      break;
    }
  }

  freeBoard();
  return 0;
}

// 初始化動態棋盤
void initBoard(int size) {
  board = (int **)malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++)
    board[i] = (int *)calloc(size, sizeof(int));
}

// 印出棋盤介面
void printBoard() {
  printf("  ");
  for (int i = 0; i < boardSize; i++)
    printf("%2d", i);
  printf("\n");
  for (int i = 0; i < boardSize; i++) {
    printf("%2d", i);
    for (int j = 0; j < boardSize; j++)
      if (board[i][j] == EMPTY)
        printf(" +");
      else if (board[i][j] == PLAYER1)
        printf(" X");
      else
        printf(" O");
    printf("\n");
  }
}

// 玩家落子
void playerMove(int player) {
  int r, c;
  while (1) {
    printf("玩家 %d 落子 (列 行): ", player);
    if (scanf("%d %d", &r, &c) == 2 && r >= 0 && r < boardSize && c >= 0 &&
        c < boardSize && board[r][c] == EMPTY) {
      board[r][c] = player;
      if (checkWin(r, c, player)) {
        system("cls");
        printBoard();
        printf("恭喜！玩家 %d 獲勝！\n", player);
        freeBoard();
        exit(0);
      }
      break;
    }
    printf("無效的位置，請重試。\n");
  }
}

// 勝負判定 (八個方向)
int checkWin(int r, int c, int player) {
  int dr[] = {1, 0, 1, 1}; // 方向：垂直, 水平, 下斜, 上斜
  int dc[] = {0, 1, 1, -1};

  for (int i = 0; i < 4; i++) {
    int count = 1;
    // 正向檢查
    for (int j = 1; j < 5; j++) {
      int nr = r + dr[i] * j, nc = c + dc[i] * j;
      if (nr < 0 || nr >= boardSize || nc < 0 || nc >= boardSize ||
          board[nr][nc] != player) {
        break;
      }
      count++;
    }
    // 反向檢查
    for (int j = 1; j < 5; j++) {
      int nr = r - dr[i] * j, nc = c - dc[i] * j;
      if (nr < 0 || nr >= boardSize || nc < 0 || nc >= boardSize ||
          board[nr][nc] != player) {
        break;
      }
      count++;
    }
    if (count >= 5)
      return 1;
  }
  return 0;
}

// 簡單 AI 邏輯 (普通模式：隨機帶點防守)
void aiMove() {
  srand(time(NULL));
  int r, c;
  // 這裡只是簡單隨機，你可以加入「檢查玩家是否有四連」的邏輯來提升難度
  do {
    r = rand() % boardSize;
    c = rand() % boardSize;
  } while (board[r][c] != EMPTY);

  board[r][c] = PLAYER2;
  if (checkWin(r, c, PLAYER2)) {
    system("cls");
    printBoard();
    printf("AI 獲勝！再接再厲。\n");
    freeBoard();
    exit(0);
  }
}

void freeBoard() {
  for (int i = 0; i < boardSize; i++)
    free(board[i]);
  free(board);
}
