#ifndef GAME_CONFIG
#define GAME_CONFIG

// console 配置
#define CONSOLE_UTF8 0xFDE9
#define CONSOLE_CLEAR_CMD "cls"

// 文字顏色定義
#define COLOR_WHITE 7
#define COLOR_YELLOW 14
#define COLOR_CYAN 11 // 用於大標題
#define COLOR_RED 12  // 用於選中項

// STONE_ 落子定義
#define STONE_BLACK 1
#define STONE_WHITE 2
#define STONE_EMPTY 0

#define STONE_CHAR_SIZE 3
#define STONE_CHAR_BLACK '●'
#define STONE_CHAR_WHITE '○'
#define STONE_CHAR_EMPTY '+'
#define STONE_CHAR_SPACE ' '

#define CONFIG_MAX_BOARD_SIZE 19

typedef struct Game {
  int board_size;
  int board[CONFIG_MAX_BOARD_SIZE][CONFIG_MAX_BOARD_SIZE];
  int current_player;
  int total_moves;
  int last_move[2];
} Game;

#endif
