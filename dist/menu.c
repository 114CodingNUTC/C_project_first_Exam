#include <conio.h> //監控使用者輸入
#include <stdio.h>
#include <windows.h>

// 定義顏色代碼
#define COLOR_WHITE 7
#define COLOR_YELLOW 14
#define COLOR_CYAN 11 // 用於大標題
#define COLOR_RED 12  // 用於選中項

// 全域變數
int currentSelection = 0; // 0: 1 Player, 1: 2 Player

// 隱藏光標 (避免閃爍)
void hideCursor() {
  HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO info;
  info.dwSize = 100;
  info.bVisible = FALSE;
  SetConsoleCursorInfo(consoleHandle, &info);
}

// 定位座標 (X: 欄, Y: 列)
void gotoxy(int x, int y) {
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 設定顏色
void setColor(int color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 繪製大標題 [GOMOKU] (ASCII Art)
void drawTitle() {
  setColor(COLOR_CYAN);
  // 將游標定位到第 2 列最左側
  gotoxy(0, 2);
  printf(
      "     [  ______   ______   .___  ___.   ______    __  ___  __    __   ]\n"
      "     [ / ___  | /  __  \\  |   \\/   |  /  __  \\  |  |/  / |  |  |  |  "
      "]\n"
      "     [| |   \\_||  |  |  | |  \\  /  | |  |  |  | |  '  /  |  |  |  |  "
      "]\n"
      "     [| |  ___ |  |  |  | |  |\\/|  | |  |  |  | |    |   |  |  |  |  "
      "]\n"
      "     [| |__/  ||  `--'  | |  |  |  | |  `--'  | |  .  \\  |  `--'  |  "
      "]\n"
      "     [ \\______| \\______/  |__|  |__|  \\______/  |__|\\__\\  "
      "\\______/   ]");

  // 下方的中文小標題保持黃色
  setColor(COLOR_YELLOW);
  gotoxy(29, 9);
  printf("=== 五 子 棋 ===");
}

// 繪製選單內容
void drawMenu() {
  // 1 Player 選項 (向右移動至 X=30 置中)
  gotoxy(30, 13);
  if (currentSelection == 0) {
    setColor(COLOR_RED);
    printf(">> 1 PLAYER <<");
  } else {
    setColor(COLOR_WHITE);
    printf("   1 PLAYER   ");
  }

  // 2 Player 選項 (向右移動至 X=30 置中)
  gotoxy(30, 15);
  if (currentSelection == 1) {
    setColor(COLOR_RED);
    printf(">> 2 PLAYER <<");
  } else {
    setColor(COLOR_WHITE);
    printf("   2 PLAYER   ");
  }

  // 底部提示 (向右移動至 X=17 置中)
  setColor(7);
  gotoxy(17, 20);
  printf("Use [UP/DOWN] to Select, [ENTER] to Start");
}

int main() {
  // 設置視窗標題與大小
  system("title GOMOKU - 五子棋 (Retro Edition)");
  system("mode con cols=75 lines=25"); // 稍微寬一點以容納標題

  hideCursor();

  // 初始繪製標題 (標題不需要每幀重繪，避免閃爍)
  system("cls");
  drawTitle();

  // 遊戲主循環
  while (1) {
    // 每幀只重繪選單部分
    drawMenu();

    // 監聽按鍵 (無阻塞)
    if (_kbhit()) {
      int ch = _getch();
      if (ch == 224) { // 方向鍵前綴
        ch = _getch(); // 讀取實際鍵值
        if (ch == 72)
          currentSelection = 0; // Up key
        if (ch == 80)
          currentSelection = 1;            // Down key
      } else if (ch == 'w' || ch == 'W') { // 支援 WS 鍵
        currentSelection = 0;
      } else if (ch == 's' || ch == 'S') {
        currentSelection = 1;
      } else if (ch == 13) { // Enter 鍵
        break;               // 跳出循環，開始遊戲
      }
    }
    Sleep(50); // 短暫延遲，減少 CPU 使用率
  }

  // 進入遊戲後的邏輯 (示例)
  system("cls");
  setColor(COLOR_WHITE);
  gotoxy(10, 10);
  printf("Entering %s Mode...",
         currentSelection == 0 ? "1 Player" : "2 Player");
  gotoxy(10, 12);
  system("pause");

  return 0;
}
