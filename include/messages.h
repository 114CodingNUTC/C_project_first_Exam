#ifndef MESSAGES_H
#define MESSAGES_H

#define MSG_WELCOME 1
#define MSG_READY 2
#define MSG_TURN_BLACK 3
#define MSG_TURN_WHITE 4
#define MSG_WIN_BLACK 5
#define MSG_WIN_WHITE 6
#define MSG_DRAW 7
#define MSG_INVALID_INPUT 8
#define MSG_OUT_OF_BOUNDS 9
#define MSG_CELL_OCCUPIED 10
#define MSG_NOT_YOUR_TURN 11
#define MSG_GAME_OVER 12
#define MSG_INTERNAL_ERROR 13

#define MSG_MENU_MODE 20
#define MSG_MENU_BOARD 21
#define MSG_MODE_1V1 22
#define MSG_MODE_1V_AI_EASY 23
#define MSG_MODE_1V_AI_MEDIUM 24
#define MSG_MODE_1V_AI_HARD 25
#define MSG_BOARD_9X9 26
#define MSG_BOARD_15X15 27
#define MSG_BOARD_19X19 28

#define MSG_INPUT_HINT 30
#define MSG_MOVE_HINT 31
#define MSG_POSITION_HINT 32

const char *msg_get(int lang, int key);

#endif
