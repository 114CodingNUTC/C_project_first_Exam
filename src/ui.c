#include "config.h"
#include "messages.h"
#include <stdio.h>

void ui_render_board(const GomokuGame *game) {
  int row;
  int col;

  if (game == 0)
    return;

  printf("Board %dx%d\n", game->board_size, game->board_size);
  for (row = 0; row < game->board_size; row++) {
    for (col = 0; col < game->board_size; col++) {
      int stone;
      stone = game->board[row][col];
      if (stone == STONE_BLACK)
        printf("X ");
      else if (stone == STONE_WHITE)
        printf("O ");
      else
        printf(". ");
    }
    printf("\n");
  }
}

void ui_show_message(int message_key) {
  printf("%s\n", msg_get(LANG_DEFAULT, message_key));
}
