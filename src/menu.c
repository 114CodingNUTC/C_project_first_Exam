#include "../include/config.h"
#include "../include/events.h"
#include "../include/functions.h"
#include "../include/message.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

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
void drawTitle(int lang) {
  setColor(COLOR_CYAN);
  // 將游標定位到第 2 列最左側
  gotoxy(0, 2);
  printf(msg_get(lang, MSG_LOGO));

  // 下方的中文小標題保持黃色
  setColor(COLOR_YELLOW);
  gotoxy(29, 9);
  printf(msg_get(lang, MSG_SUBTITLE));
}

int game_menu(int lang) {
  hideCursor();
  drawTitle(lang);
  return EV_GAME_NONE;
}
