#ifndef MESSAGES_H
#define MESSAGES_H

#define LANG_ZH_TW 1

// MSG_ KEY 定義
// 視窗
#define MSG_WINDOW_TITLE 2
#define MSG_WINDOW_SIZE 3

#define MSG_WELCOME -1
#define MSG_LOGO 1
#define MSG_SUBTITLE 4

const char *msg_get(int lang, int key);

#endif
