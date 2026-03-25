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

const char *msg_get(int lang, int key);

#endif
