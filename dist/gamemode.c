#include <stdio.h>
#include <stdlib.h> // 提供 system("pause")

int main() {
  int mode;

  printf("選擇模式: [1]雙人 [2]AI: ");

  // 讀取並防呆
  while (scanf("%d", &mode) != 1 || (mode < 1 || mode > 2)) {
    printf("輸入錯誤，請輸入 1 或 2: ");
    while (getchar() != '\n')
      ; // 清除錯字
  }

  if (mode == 1)
    printf(">> 已進入 雙人對戰\n");
  else
    printf(">> 已進入 人機對戰\n");

  // Dev-C++ 必備，防止視窗閃退
  system("pause");

  return 0;
}
