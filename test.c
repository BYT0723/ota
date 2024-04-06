#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 信号处理函数
void handle_interrupt(int signum) {
  printf("\n收到中断信号，程序退出\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  // 注册信号处理函数
  signal(SIGINT, handle_interrupt);
  signal(SIGTERM, handle_interrupt);

  printf("这是一个测试程序，在收到中断信号时退出\n");

  // 挂起进程，等待信号
  pause();

  return 0;
}
