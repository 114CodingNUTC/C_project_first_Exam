#include "config.h"
#include "functions.h"
#include "events.h"
#include "messages.h"
#include <stdio.h>

int main(void){
  int r,c,ev,msg,res;
  GomokuGame g;
  game_reset(&g, CFG_BOARD_SIZE_19);

  printf("parse s19=%d ", input_parse_move("s19", 19, &r, &c));
  printf("r=%d c=%d\n", r, c);

  printf("parse 19s=%d ", input_parse_move("19s", 19, &r, &c));
  printf("r=%d c=%d\n", r, c);

  printf("parse t19=%d\n", input_parse_move("t19", 19, &r, &c));

  res = try_place_stone(&g, STONE_BLACK, 18, 18, &ev, &msg);
  printf("place (18,18): res=%d ev=%d msg=%d\n", res, ev, msg);

  res = try_place_stone(&g, STONE_WHITE, 18, 18, &ev, &msg);
  printf("place occupied: res=%d ev=%d msg=%d\n", res, ev, msg);

  return 0;
}
