int **board;
int boardSize;

// 根據用戶輸入初始化棋盤
void initBoard(int size) {
  boardSize = size;
  board = (int **)malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++)
    board[i] = (int *)calloc(size, sizeof(int)); // 初始化為 0 (空位)
}
